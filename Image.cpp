#include "Image.h"

Image::Image(QOpenGLExtraFunctions* gl, int width, int height)
	: m_gl(gl)
	, m_width(width)
	, m_height(height)
	, m_rgba(width*height*4)
	, m_thickness(width*height)
	, m_gl_rgba(gl)
	, m_gl_thickness(gl)
	, m_gl_norm(gl)
{
	memset(m_rgba.data(), 255, width * height * 4);
	m_gl_norm.create(m_width, m_height);

	m_gl->glGenFramebuffers(1, &m_fbo);


}

Image::~Image()
{
	m_gl->glDeleteFramebuffers(1, &m_fbo);
}

void Image::update_textures()
{
	m_gl_rgba.upload(m_width, m_height, m_rgba.data());
	m_gl_thickness.upload(m_width, m_height, m_thickness.data());

	m_gl->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	m_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gl_rgba.tex_id, 0);
	m_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gl_thickness.tex_id, 0);
}
