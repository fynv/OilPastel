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
class BlitImage;

class Stroke;

class GLCanvas : public QOpenGLWidget
{
	Q_OBJECT
public:
	GLCanvas(QWidget* parent);
	~GLCanvas();

	int m_image_width = 1024;
	int m_image_height = 768;

	glm::vec3 m_light_dir = glm::vec3(0.5f, 0.5f, 1.0f);
	
	glm::vec4 stroke_color = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);
	float stroke_radius = 20.0f;
	float stroke_grain_size = 3.0f;
	float stroke_roughness = 1.0f;
	float stroke_hardness = 0.0f;


protected:
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;

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
	std::unique_ptr<BlitImage> m_bi;

	std::vector<glm::vec2> m_points;
	std::unique_ptr<Stroke> m_stroke;
	std::unique_ptr<Image> m_tmp_image;

	

};
