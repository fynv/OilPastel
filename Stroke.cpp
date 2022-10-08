#include "Stroke.h"
#include "Operations.h"
#include "routines/Blur.h"

Stroke::Stroke(QOpenGLExtraFunctions* gl, int width, int height, BlurRow* br, BlurCol* bc, float filterRadius, float contrast, int numApproxPasses)
	: m_gl(gl)
	, m_width(width)
	, m_height(height)
	, tex_pattern(m_gl)
	, tex_outline(m_gl)
	, tex_outline_4x(m_gl)
{
	std::vector<float> pattern(width * height);
	RandomThickness(m_width, m_height, pattern);
	tex_pattern.upload(m_width, m_height, pattern.data());
	br->filter(tex_pattern.tex_id, height, filterRadius, contrast, numApproxPasses);
	bc->filter(tex_pattern.tex_id, width, filterRadius, contrast, numApproxPasses);

	m_gl->glGenFramebuffers(1, &m_fbo_outline);
	m_gl->glGenFramebuffers(1, &m_fbo_outline_4x);

	m_gl->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_outline);
	tex_outline.create(m_width, m_height);
	m_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_outline.tex_id, 0);

	m_gl->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_outline_4x);
	m_gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex_outline_4x.tex_id);
	m_gl->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_R32F, m_width, m_height, true);
	m_gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	m_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex_outline_4x.tex_id, 0);
}

Stroke::~Stroke()
{
	m_gl->glDeleteFramebuffers(1, &m_fbo_outline_4x);
	m_gl->glDeleteFramebuffers(1, &m_fbo_outline);
}

void Stroke::bind_buffer()
{
	m_gl->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_outline_4x);
}

void Stroke::resolve_msaa()
{
	m_gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_outline);
	m_gl->glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	m_gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_outline);
}
