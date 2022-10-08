#include <QOpenGLExtraFunctions>
#include <string>
#include "StrokeDrawCurve.h"


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

static std::string g_geo =
R"(#version 430
layout(lines) in;
layout(triangle_strip, max_vertices = 104) out;
layout (location = 0) out float value;

layout (location = 0) uniform int width;
layout (location = 1) uniform int height;
layout (location = 2) uniform float radius;

vec2 toScreen(in vec4 pos)
{
	return (pos.xy*0.5 + 0.5)* vec2(width, height);
}

vec4 fromScreen(in vec2 pos)
{
	return vec4(pos/vec2(width, height) * 2.0 -1.0, 0.0, 1.0);
}

const float pi = 3.14159265359;

void main()
{
	vec2 pos0 = toScreen(gl_in[0].gl_Position);
	vec2 pos1 = toScreen(gl_in[1].gl_Position);
	vec2 norm = normalize(pos1 - pos0);
	vec2 norm2 = vec2(-norm.y, norm.x);
	norm*=radius;
	norm2*=radius;

	for (int i=0; i<16; i++)
	{
		float alpha0 = pi/16.0*float(i);
		float alpha1 = pi/16.0*float(i+1);
		
		{
			vec2 v0 = pos0 + cos(alpha0)*norm2 - sin(alpha0)*norm;
			vec2 v1 = pos0 + cos(alpha1)*norm2 - sin(alpha1)*norm;
		
			gl_Position = fromScreen(pos0);
			value= 1.0;
			EmitVertex();

			gl_Position = fromScreen(v0);
			value= 0.0;
			EmitVertex();

			gl_Position = fromScreen(v1);
			value= 0.0;
			EmitVertex();
		
			EndPrimitive();
		}
		
		{
			vec2 v0 = pos1 - cos(alpha0)*norm2 + sin(alpha0)*norm;
			vec2 v1 = pos1 - cos(alpha1)*norm2 + sin(alpha1)*norm;
		
			gl_Position = fromScreen(pos1);
			value= 1.0;
			EmitVertex();

			gl_Position = fromScreen(v0);
			value= 0.0;
			EmitVertex();

			gl_Position = fromScreen(v1);
			value= 0.0;
			EmitVertex();
		
			EndPrimitive();
		}
	}

	{
		gl_Position = fromScreen(pos0 - norm2);
		value= 0.0;
		EmitVertex();

		gl_Position = fromScreen(pos1 - norm2);
		value= 0.0;
		EmitVertex();

		gl_Position = fromScreen(pos0);
		value= 1.0;
		EmitVertex();

		gl_Position = fromScreen(pos1);
		value= 1.0;
		EmitVertex();

		EndPrimitive();

	}
		
	{
		gl_Position = fromScreen(pos1 + norm2);
		value= 0.0;
		EmitVertex();

		gl_Position = fromScreen(pos0 + norm2);
		value= 0.0;
		EmitVertex();

		gl_Position = fromScreen(pos1);
		value= 1.0;
		EmitVertex();

		gl_Position = fromScreen(pos0);
		value= 1.0;
		EmitVertex();

		EndPrimitive();
	}
	
}
)";


static std::string g_frag =
R"(#version 430
layout (location = 0) in float value;
layout (location = 0) out vec4 outColor;

layout (location = 3) uniform float hardness;
void main()
{	
	float v = value>0.0? (hardness<1.0 ? pow(value, 1.0 - hardness):1.0) : 0.0;
	outColor = vec4(v, v, v, 1.0);
}
)";


StrokeDrawCurve::StrokeDrawCurve(QOpenGLExtraFunctions* gl) : m_gl(gl)
{
	GLShader vert_shader(m_gl, GL_VERTEX_SHADER, g_vertex.c_str());
	GLShader geo_shader(m_gl, GL_GEOMETRY_SHADER, g_geo.c_str());
	GLShader frag_shader(m_gl, GL_FRAGMENT_SHADER, g_frag.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, vert_shader, geo_shader, frag_shader));
}

void StrokeDrawCurve::draw(int width, int height, float radius, float hardness, int num_verts, glm::vec2* verts)
{
	GLBuffer buf(m_gl, sizeof(glm::vec2) * num_verts);
	buf.upload(verts);

	m_gl->glEnable(GL_BLEND);
	m_gl->glBlendEquation(GL_MAX);

	m_gl->glViewport(0, 0, width, height);
	m_gl->glDisable(GL_DEPTH_TEST);

	m_gl->glUseProgram(m_prog->m_id);
	m_gl->glUniform1i(0, width);
	m_gl->glUniform1i(1, height);
	m_gl->glUniform1f(2, radius);
	m_gl->glUniform1f(3, hardness);

	m_gl->glBindBuffer(GL_ARRAY_BUFFER, buf.m_id);
	m_gl->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	m_gl->glEnableVertexAttribArray(0);

	m_gl->glDrawArrays(GL_LINE_STRIP, 0, num_verts);
	m_gl->glUseProgram(0);

	m_gl->glBlendEquation(GL_FUNC_ADD);
}



