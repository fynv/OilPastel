#include <QPainter>
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

	m_blur_row = std::unique_ptr<BlurRow>(new BlurRow(&m_gl, 1024));
	m_blur_col = std::unique_ptr<BlurCol>(new BlurCol(&m_gl, 768));
	m_comp_norm = std::unique_ptr<CompNorm>(new CompNorm(&m_gl));

	m_dtex = std::unique_ptr<DrawTexture>(new DrawTexture(&m_gl));
	m_dt = std::unique_ptr<DrawThickness>(new DrawThickness(&m_gl));
	m_dl = std::unique_ptr<DrawLighted>(new DrawLighted(&m_gl));
	m_sdt = std::unique_ptr<StrokeDrawTriangles>(new StrokeDrawTriangles(&m_gl));
	m_sb = std::unique_ptr<StrokeBlend>(new StrokeBlend(&m_gl));
	m_sdc = std::unique_ptr<StrokeDrawCurve>(new StrokeDrawCurve(&m_gl));

	
	m_image = std::unique_ptr<Image>(new Image(&m_gl, 1024, 768));
	RandomThickness(m_image->m_width, m_image->m_height, m_image->m_thickness);
	m_image->update_textures();
	
	m_blur_row->filter(m_image->m_gl_thickness.tex_id, 768, 3.0f, 0.3f, 3);
	m_blur_col->filter(m_image->m_gl_thickness.tex_id, 1024, 3.0f, 0.3f, 3);
	

	{
		m_stroke = std::unique_ptr<Stroke>(new Stroke(&m_gl, 1024, 768, m_blur_row.get(), m_blur_col.get(), 3.0f, 1.0f, 3));
		glm::vec2 verts[] = { { 100.0f, 100.0f}, { 100.0f, 300.0f}, { 300.0f, 100.0f} };
		m_stroke->bind_buffer();
		m_sdt->draw(1024, 768, 3, verts);
		m_stroke->resolve_msaa();
		m_stroke->color = { 1.0f, 0.0f,0.0f, 0.5f };
		m_sb->Blend(*m_image, *m_stroke);
	}


	{
		m_stroke = std::unique_ptr<Stroke>(new Stroke(&m_gl, 1024, 768, m_blur_row.get(), m_blur_col.get(), 3.0f, 1.0f, 3));
		glm::vec2 verts[] = { { 120.0f, 120.0f}, { 120.0f, 320.0f}, { 320.0f, 120.0f} };
		m_stroke->bind_buffer();
		m_sdt->draw(1024, 768, 3, verts);
		m_stroke->resolve_msaa();
		m_stroke->color = { 0.0f, 1.0f,0.0f, 0.5f };
		m_sb->Blend(*m_image, *m_stroke);
	}

	{
		m_stroke = std::unique_ptr<Stroke>(new Stroke(&m_gl, 1024, 768, m_blur_row.get(), m_blur_col.get(), 3.0f, 1.0f, 3));
		glm::vec2 verts[] = { { 140.0f, 140.0f}, { 140.0f, 340.0f}, { 340.0f, 140.0f} };
		m_stroke->bind_buffer();
		m_sdt->draw(1024, 768, 3, verts);
		m_stroke->resolve_msaa();
		m_stroke->color = { 0.0f, 0.0f, 1.0f, 0.5f };
		m_sb->Blend(*m_image, *m_stroke);
	}

	{
		m_stroke = std::unique_ptr<Stroke>(new Stroke(&m_gl, 1024, 768, m_blur_row.get(), m_blur_col.get(), 3.0f, 1.0f, 3));

		glm::vec2 verts[] = { { 700.0f, 100.0f}, { 600.0f, 200.0f}, { 650.0f, 350.0f}, { 550.0f, 450.0f} };
		m_stroke->bind_buffer();
		m_sdc->draw(1024, 768, 20.0, 0.0f, 4, verts);
		m_stroke->resolve_msaa();
		m_stroke->color = { 1.0f, 0.0f,0.0f, 0.5f };
		m_sb->Blend(*m_image, *m_stroke);
	}

	{
		m_stroke = std::unique_ptr<Stroke>(new Stroke(&m_gl, 1024, 768, m_blur_row.get(), m_blur_col.get(), 3.0f, 1.0f, 3));

		glm::vec2 verts[] = { { 500.0f, 100.0f}, { 700.0f, 600.0f} };
		m_stroke->bind_buffer();
		m_sdc->draw(1024, 768, 20.0, 0.0f, 2, verts);
		m_stroke->resolve_msaa();
		m_stroke->color = { 0.0f, 0.0f, 1.0f, 0.5f };
		m_sb->Blend(*m_image, *m_stroke);
	}
	
	m_comp_norm->comp(m_image->m_gl_thickness.tex_id, m_image->m_gl_norm.tex_id, 1024, 768, 0.25f);
	m_image->m_gl_norm.genMIPs();
}

void GLCanvas::paintGL()
{
	QPainter painter;
	painter.begin(this);

	painter.beginNativePainting();
	m_gl.glViewport(0, 0, this->width(), this->height());
	m_gl.glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	m_gl.glClear(GL_COLOR_BUFFER_BIT);

	//m_dt->render(m_image->m_gl_thickness.tex_id, 0, 0, this->width(), this->height());
	//m_dtex->render(m_image->m_gl_norm.tex_id, 0, 0, this->width(), this->height());
	m_dl->render(*m_image, 0, 0, this->width(), this->height(), m_light_dir);

	//m_dt->render(m_stroke->tex_outline.tex_id, 0, 0, this->width(), this->height());

	painter.endNativePainting();

}