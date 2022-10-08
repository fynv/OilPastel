#pragma once

#include <glm.hpp>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <memory>

class Image;
class DrawTexture;
class DrawThickness;
class BlurRow;
class BlurCol;
class CompNorm;
class DrawLighted;
class StrokeDrawTriangles;
class StrokeBlend;
class StrokeDrawCurve;

class Stroke;

class GLCanvas : public QOpenGLWidget
{
	Q_OBJECT
public:
	GLCanvas(QWidget* parent);
	~GLCanvas();

	glm::vec3 m_light_dir = glm::vec3(0.5f, 0.5f, 1.0f);

protected:
	virtual void initializeGL() override;
	virtual void paintGL() override;

private:
	QOpenGLExtraFunctions m_gl;

	std::unique_ptr<Image> m_image;
	std::unique_ptr<DrawTexture> m_dtex;
	std::unique_ptr<DrawThickness> m_dt;
	std::unique_ptr<BlurRow> m_blur_row;
	std::unique_ptr<BlurCol> m_blur_col;
	std::unique_ptr<CompNorm> m_comp_norm;
	std::unique_ptr<DrawLighted> m_dl;
	std::unique_ptr<StrokeDrawTriangles> m_sdt;
	std::unique_ptr<StrokeBlend> m_sb;
	std::unique_ptr<StrokeDrawCurve> m_sdc;

	std::unique_ptr<Stroke> m_stroke;

	

};
