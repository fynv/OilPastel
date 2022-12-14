#include <QOpenGLExtraFunctions>
#include <string>
#include "DrawTexture.h"

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
layout (location = 0) uniform sampler2D uTex;
void main()
{
	vec4 col = texture(uTex, vUV);
	outColor = vec4(col.xyz*col.w, col.w);
}
)";


static std::string g_frag_premult =
R"(#version 430
layout (location = 0) in vec2 vUV;
layout (location = 0) out vec4 outColor;
layout (location = 0) uniform sampler2D uTex;
void main()
{
	vec4 col = texture(uTex, vUV);
	outColor = vec4(col.xyz, col.w);
}
)";


DrawTexture::DrawTexture(QOpenGLExtraFunctions* gl, bool premult) : m_gl(gl)
{
	std::string s_frag = premult ? g_frag_premult : g_frag;
	GLShader vert_shader(m_gl, GL_VERTEX_SHADER, g_vertex.c_str());
	GLShader frag_shader(m_gl, GL_FRAGMENT_SHADER, s_frag.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, vert_shader, frag_shader));
}

void DrawTexture::render(unsigned tex_id, int x, int y, int width, int height, bool blending)
{
	m_gl->glViewport(x, y, width, height);
	m_gl->glDisable(GL_DEPTH_TEST);

	if (blending)
	{
		m_gl->glEnable(GL_BLEND);
		m_gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		m_gl->glDisable(GL_BLEND);
	}

	m_gl->glUseProgram(m_prog->m_id);
	m_gl->glActiveTexture(GL_TEXTURE0);
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glUniform1i(0, 0);
	m_gl->glDrawArrays(GL_TRIANGLES, 0, 3);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
	m_gl->glUseProgram(0);

}
