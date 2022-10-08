#pragma once 

#include <cstdint>
#include <vector>
#include <QOpenGLExtraFunctions>
#include "GLUtils.h"

class Image
{
public:
	Image(QOpenGLExtraFunctions* gl, int width, int height);
	~Image();
	
	QOpenGLExtraFunctions* m_gl;
	int m_width, m_height;
	std::vector<uint8_t> m_rgba;
	std::vector<float> m_thickness;

	GL2DRGBA m_gl_rgba;
	GL2DThickness m_gl_thickness;
	GL2DRGBA m_gl_norm;

	unsigned m_fbo;

	void create_textures();
	void update_textures();	
};
