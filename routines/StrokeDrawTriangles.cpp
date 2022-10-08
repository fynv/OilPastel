#include <QOpenGLExtraFunctions>
#include <string>
#include "StrokeDrawTriangles.h"


static std::string g_vertex =
R"(#version 430

layout (location = 0) in vec2 aPos;

layout (location = 0) uniform int width;
layout (location = 1) uniform int height;

void main()
{
	vec3 pos = vec3(aPos/vec2(width, height) * 2.0 - 1.0, 0.0);
	pos.y = -pos.y;
	gl_Position = vec4(pos, 1.0);
}
)";


static std::string g_frag =
R"(#version 430
layout (location = 0) out vec4 outColor;
void main()
{	
	outColor = vec4(1.0);
}
)";



StrokeDrawTriangles::StrokeDrawTriangles(QOpenGLExtraFunctions* gl) : m_gl(gl)
{
	GLShader vert_shader(m_gl, GL_VERTEX_SHADER, g_vertex.c_str());
	GLShader frag_shader(m_gl, GL_FRAGMENT_SHADER, g_frag.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, vert_shader, frag_shader));
}

void StrokeDrawTriangles::draw(int width, int height, int num_verts, glm::vec2* verts)
{
	GLBuffer buf(m_gl, sizeof(glm::vec2) * num_verts);
	buf.upload(verts);

	m_gl->glViewport(0, 0, width, height);
	m_gl->glDisable(GL_DEPTH_TEST);

	m_gl->glUseProgram(m_prog->m_id);
	m_gl->glUniform1i(0, width);
	m_gl->glUniform1i(1, height);

	m_gl->glBindBuffer(GL_ARRAY_BUFFER, buf.m_id);
	m_gl->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	m_gl->glEnableVertexAttribArray(0);
	
	m_gl->glDrawArrays(GL_TRIANGLES, 0, num_verts);
	m_gl->glUseProgram(0);
}

