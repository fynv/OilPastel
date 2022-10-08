#include <QOpenGLExtraFunctions>
#include <string>
#include "CompNorm.h"


static std::string g_compute =
R"(#version 430

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding=0, r32f) uniform image2D tex_thickness;
layout(binding=1, rgba8) uniform image2D tex_norm;

layout (location = 0) uniform int width;
layout (location = 1) uniform int height;
layout (location = 2) uniform float pix_size;

void main()
{
	ivec3 id = ivec3(gl_GlobalInvocationID);
	if (id.x >= width || id.y >= height) return;

	float t0 = imageLoad(tex_thickness, ivec2(id.x,id.y)).x;
	float t1 = -1.0;
	float t2 = -1.0;
	float t3 = -1.0;
	float t4 = -1.0;
	if (id.x > 0)
	{
		t1 = imageLoad(tex_thickness, ivec2(id.x - 1, id.y)).x;
	}
	if (id.x < width - 1)
	{
		t2 = imageLoad(tex_thickness, ivec2(id.x + 1, id.y)).x;
	}
	if (id.y > 0)
	{
		t3 = imageLoad(tex_thickness, ivec2(id.x, id.y - 1)).x;
	}
	if (id.y < height - 1)
	{
		t4 = imageLoad(tex_thickness, ivec2(id.x, id.y + 1)).x;
	}

	vec3 norm = vec3(0.0);

	if (t1>=0.0)
	{
		if (t3>=0.0)
		{
			vec3 v1 = vec3(-pix_size, 0.0, t1 - t0);
			vec3 v2 = vec3(0.0, -pix_size, t3 - t0);			
			vec3 n = normalize(cross(v1, v2));
			norm += n;
		}

		if (t4>=0.0)
		{
			vec3 v1 = vec3(0.0, pix_size, t4 - t0);
			vec3 v2 = vec3(-pix_size, 0.0, t1 - t0);			
			vec3 n = normalize(cross(v1, v2));
			norm += n;
		}
	}

	if (t2>=0.0)
	{
		if (t3>=0.0)
		{
			vec3 v1 = vec3(0.0, -pix_size, t3 - t0);
			vec3 v2 = vec3(pix_size, 0.0, t2 - t0);			
			vec3 n = normalize(cross(v1, v2));
			norm += n;
		}

		if (t4>=0.0)
		{
			vec3 v1 = vec3(pix_size, 0.0, t2 - t0);
			vec3 v2 = vec3(0.0, pix_size, t4 - t0);			
			vec3 n = normalize(cross(v1, v2));
			norm += n;
		}
	}

	norm = normalize(norm);
	norm = 0.5*norm + 0.5;
	
	imageStore(tex_norm, id.xy, vec4( norm, 1.0 ) );
}

)";

CompNorm::CompNorm(QOpenGLExtraFunctions* gl): m_gl(gl)
{
	GLShader compute_shader(m_gl, GL_COMPUTE_SHADER, g_compute.c_str());
	m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, compute_shader));
}

void CompNorm::comp(unsigned tex_id_thickness, unsigned tex_id_norm, int width, int height, float pix_size)
{
	m_gl->glUseProgram(m_prog->m_id);
	m_gl->glUniform1i(0, width);
	m_gl->glUniform1i(1, height);
	m_gl->glUniform1f(2, pix_size);
	
	m_gl->glBindImageTexture(0, tex_id_thickness, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
	m_gl->glBindImageTexture(1, tex_id_norm, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	m_gl->glDispatchCompute((width+7)/8, (height + 7) / 8, 1);	
	m_gl->glUseProgram(0);

	m_gl->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}