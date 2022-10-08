#include <QOpenGLExtraFunctions>
#include <string>
#include "DrawThickness.h"



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

static std::string g_vertex_flip =
R"(#version 430
layout (location = 0) out vec2 vUV;
void main()
{
	vec2 grid = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec2 vpos = grid * vec2(2.0, 2.0) + vec2(-1.0, -1.0);
	gl_Position = vec4(vpos, 1.0, 1.0);
	vUV = vec2(grid.x, 1.0 - grid.y);
}
)";


static std::string g_frag =
R"(#version 430
layout (location = 0) in vec2 vUV;
layout (location = 0) out vec4 outColor;
layout (location = 0) uniform sampler2D uTex;
void main()
{
	float lum = texture(uTex, vUV).x;
	outColor = vec4(lum, lum, lum, 1.0);
}
)";


DrawThickness::DrawThickness(QOpenGLExtraFunctions* gl, bool flipY) : m_gl(gl)
{
	std::string s_vertex = flipY ? g_vertex_flip : g_vertex;	
	GLShader vert_shader(m_gl, GL_VERTEX_SHADER, s_vertex.c_str());
	GLShader frag_shader(m_gl, GL_FRAGMENT_SHADER, g_frag.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, vert_shader, frag_shader));
}

void DrawThickness::render(unsigned tex_id, int x, int y, int width, int height)
{
	m_gl->glViewport(x, y, width, height);
	m_gl->glDisable(GL_DEPTH_TEST);

	m_gl->glDisable(GL_BLEND);

	m_gl->glUseProgram(m_prog->m_id);
	m_gl->glActiveTexture(GL_TEXTURE0);
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glUniform1i(0, 0);
	m_gl->glDrawArrays(GL_TRIANGLES, 0, 3);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
	m_gl->glUseProgram(0);

}
