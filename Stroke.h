#pragma once

#include <QOpenGLExtraFunctions>
#include <glm.hpp>
#include "GLUtils.h"

class BlurRow;
class BlurCol;
class Stroke
{
public:
	Stroke(QOpenGLExtraFunctions* gl, int width, int height, BlurRow* br, BlurCol* bc, float filterRadius, float contrast, int numApproxPasses);
	~Stroke();
	
	QOpenGLExtraFunctions* m_gl;
	int m_width, m_height;

	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.9f);

	GL2DThickness tex_pattern;
	GL2DThickness tex_outline;
	GL2DThickness tex_outline_4x;

	unsigned m_fbo_outline;
	unsigned m_fbo_outline_4x;

	void bind_buffer();
	void resolve_msaa();

};

