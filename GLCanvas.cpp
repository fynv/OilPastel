#include <QPainter>
#include <QMouseEvent>
#include <glm.hpp>

#include "Image.h"
#include "Operations.h"
#include "GLCanvas.h"
#include "routines/DrawTexture.h"
#include "routines/DrawThickness.h"
#include "routines/Blur.h"
#include "routines/CompNorm.h"
#include "routines/DrawLighted.h"
#include "routines/StrokeDrawTriangles.h"
#include "routines/StrokeBlend.h"
#include "routines/StrokeDrawCurve.h"
#include "routines/BlitImage.h"

#include "Stroke.h"


GLCanvas::GLCanvas(QWidget* parent)
	: QOpenGLWidget(parent)
{
	
}

GLCanvas::~GLCanvas()
{
	
}

void GLCanvas::initializeGL()
{
	m_gl.initializeOpenGLFunctions();

	m_blur_row = std::unique_ptr<BlurRow>(new BlurRow(&m_gl, m_image_width));
	m_blur_col = std::unique_ptr<BlurCol>(new BlurCol(&m_gl, m_image_height));
	m_comp_norm = std::unique_ptr<CompNorm>(new CompNorm(&m_gl));

	m_dtex = std::unique_ptr<DrawTexture>(new DrawTexture(&m_gl));
	m_dt = std::unique_ptr<DrawThickness>(new DrawThickness(&m_gl));
	m_dl = std::unique_ptr<DrawLighted>(new DrawLighted(&m_gl));
	m_sdt = std::unique_ptr<StrokeDrawTriangles>(new StrokeDrawTriangles(&m_gl));
	m_sb = std::unique_ptr<StrokeBlend>(new StrokeBlend(&m_gl));
	m_sdc = std::unique_ptr<StrokeDrawCurve>(new StrokeDrawCurve(&m_gl));
	m_bi = std::unique_ptr<BlitImage>(new BlitImage(&m_gl));

	
	m_image = std::unique_ptr<Image>(new Image(&m_gl, m_image_width, m_image_height));
	RandomThickness(m_image->m_width, m_image->m_height, m_image->m_thickness);
	m_image->update_textures();
	
	m_blur_row->filter(m_image->m_gl_thickness.tex_id, m_image_height, 3.0f, 0.3f, 3);
	m_blur_col->filter(m_image->m_gl_thickness.tex_id, m_image_width, 3.0f, 0.3f, 3);
	
	m_comp_norm->comp(m_image->m_gl_thickness.tex_id, m_image->m_gl_norm.tex_id, m_image_width, m_image_height, 0.25f);
}

void GLCanvas::paintGL()
{	
	QPainter painter;
	painter.begin(this);

	painter.beginNativePainting();	
	m_gl.glEnable(GL_FRAMEBUFFER_SRGB);
	m_gl.glViewport(0, 0, this->width(), this->height());
	m_gl.glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	m_gl.glClear(GL_COLOR_BUFFER_BIT);

	if (m_points.size() > 1)
	{
		m_dl->render(*m_tmp_image, 0, 0, this->width(), this->height(), m_light_dir);
	}
	else
	{
		m_dl->render(*m_image, 0, 0, this->width(), this->height(), m_light_dir);

	}

	painter.endNativePainting();

}

void GLCanvas::mousePressEvent(QMouseEvent* event)
{
	QPointF p = event->position();
	m_points.clear();
	m_points.push_back({ p.x(), p.y() });

	this->makeCurrent();
	m_tmp_image = std::unique_ptr<Image>(new Image(&m_gl, m_image_width, m_image_height));
	m_tmp_image->create_textures();

	m_stroke = std::unique_ptr<Stroke>(new Stroke(&m_gl, m_image_width, m_image_height, m_blur_row.get(), m_blur_col.get(), 3.0f, 1.0f, 3));
	m_stroke->color = { 1.0f, 0.0f, 0.0f, 0.5f };

}

void GLCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_points.size() > 1)
	{
		this->makeCurrent();
		m_bi->Copy(*m_image, *m_tmp_image);
		m_comp_norm->comp(m_image->m_gl_thickness.tex_id, m_image->m_gl_norm.tex_id, m_image_width, m_image_height, 0.25f);
	}
	m_points.clear();
	m_tmp_image = nullptr;
	m_stroke = nullptr;
	update();
}

void GLCanvas::mouseMoveEvent(QMouseEvent* event)
{
	QPointF p = event->position();
	m_points.push_back({ p.x(), p.y() });

	this->makeCurrent();
	m_bi->Copy(*m_tmp_image, *m_image);

	m_stroke->bind_buffer();
	m_gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_gl.glClear(GL_COLOR_BUFFER_BIT);
	m_sdc->draw(m_image_width, m_image_height, 20.0, 0.0f, (int)m_points.size(), m_points.data());
	m_stroke->resolve_msaa();	
	m_sb->Blend(*m_tmp_image, *m_stroke);

	m_comp_norm->comp(m_tmp_image->m_gl_thickness.tex_id, m_tmp_image->m_gl_norm.tex_id, m_image_width, m_image_height, 0.25f);
	update();
}
