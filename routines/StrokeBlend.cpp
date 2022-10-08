#include <QOpenGLExtraFunctions>
#include <string>
#include "StrokeBlend.h"

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

layout (location = 0) uniform vec4 uColor;
layout (location = 1) uniform sampler2D uPattern;
layout (location = 2) uniform sampler2D uOutline;
void main()
{
	float thickness = texture(uPattern, vUV).x * texture(uOutline, vUV).x;
	float alpha = thickness>0.0 ? 1.0 -  pow(1.0 - uColor.w, thickness) : 0.0;
	outColor = vec4(uColor.xyz, alpha);
	outThickness = vec4(thickness);
}
)";



StrokeBlend::StrokeBlend(QOpenGLExtraFunctions* gl) : m_gl(gl)
{
	GLShader vert_shader(m_gl, GL_VERTEX_SHADER, g_vertex.c_str());
	GLShader frag_shader(m_gl, GL_FRAGMENT_SHADER, g_frag.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, vert_shader, frag_shader));
}

void StrokeBlend::Blend(Image& img, Stroke& stroke)
{
	m_gl->glBindFramebuffer(GL_FRAMEBUFFER, img.m_fbo);
	
	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	m_gl->glDrawBuffers(2, drawBuffers);

	m_gl->glViewport(0, 0, img.m_width, img.m_height);
	m_gl->glEnable(GL_BLEND);
	m_gl->glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_gl->glBlendFunci(1, GL_ONE, GL_ONE);

	m_gl->glUseProgram(m_prog->m_id);

	m_gl->glUniform4fv(0, 1, (float*)&stroke.color);	

	m_gl->glActiveTexture(GL_TEXTURE0);
	m_gl->glBindTexture(GL_TEXTURE_2D, stroke.tex_pattern.tex_id);
	m_gl->glUniform1i(1, 0);

	m_gl->glActiveTexture(GL_TEXTURE1);
	m_gl->glBindTexture(GL_TEXTURE_2D, stroke.tex_outline.tex_id);
	m_gl->glUniform1i(2, 1);

	m_gl->glDrawArrays(GL_TRIANGLES, 0, 3);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
	m_gl->glUseProgram(0);

	img.m_gl_rgba.genMIPs();

}

