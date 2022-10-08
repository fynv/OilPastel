#include <QOpenGLExtraFunctions>
#include <string>
#include "BlitImage.h"


static std::string g_vertex =
R"(#version 430
layout (location = 0) out vec2 vUV;
void main()
{
	vec2 grid = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec2 vpos = grid * vec2(2.0, 2.0) + vec2(-1.0, -1.0);
	gl_Position = vec4(vpos, 1.0, 1.0);
	vUV = vec2(grid.x, grid.y);
}
)";


static std::string g_frag =
R"(#version 430
layout (location = 0) in vec2 vUV;
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outThickness;

layout (location = 0) uniform sampler2D uColor;
layout (location = 1) uniform sampler2D uThickness;
void main()
{
	outColor = texture(uColor, vUV);
	float t = texture(uThickness, vUV).x;
	outThickness = vec4(t,t,t,1.0);
}
)";


BlitImage::BlitImage(QOpenGLExtraFunctions* gl) : m_gl(gl)
{
	GLShader vert_shader(m_gl, GL_VERTEX_SHADER, g_vertex.c_str());
	GLShader frag_shader(m_gl, GL_FRAGMENT_SHADER, g_frag.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, vert_shader, frag_shader));
}



void BlitImage::Copy(Image& img_out, Image& img_in)
{
	m_gl->glBindFramebuffer(GL_FRAMEBUFFER, img_out.m_fbo);
	m_gl->glEnable(GL_FRAMEBUFFER_SRGB);

	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	m_gl->glDrawBuffers(2, drawBuffers);

	m_gl->glViewport(0, 0, img_out.m_width, img_out.m_height);
	m_gl->glDisable(GL_BLEND);
	
	m_gl->glUseProgram(m_prog->m_id);

	m_gl->glActiveTexture(GL_TEXTURE0);
	m_gl->glBindTexture(GL_TEXTURE_2D, img_in.m_gl_rgba.tex_id);
	m_gl->glUniform1i(0, 0);

	m_gl->glActiveTexture(GL_TEXTURE1);
	m_gl->glBindTexture(GL_TEXTURE_2D, img_in.m_gl_thickness.tex_id);
	m_gl->glUniform1i(1, 1);

	m_gl->glDrawArrays(GL_TRIANGLES, 0, 3);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
	m_gl->glUseProgram(0);

}

