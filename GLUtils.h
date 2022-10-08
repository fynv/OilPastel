#pragma once

#include <cstdint>

class QOpenGLExtraFunctions;

struct GLBuffer
{
	QOpenGLExtraFunctions* m_gl;
	unsigned m_id = -1;
	unsigned m_target = 0x8892;
	size_t m_size = 0;
	GLBuffer(QOpenGLExtraFunctions* gl, size_t size, unsigned target = 0x8892 /*GL_ARRAY_BUFFER*/);
	~GLBuffer();
	void upload(const void* data);
	const GLBuffer& operator = (const GLBuffer& in);
};


struct GLShader
{
	QOpenGLExtraFunctions* m_gl;
	unsigned m_type = 0;
	unsigned m_id = -1;
	GLShader(QOpenGLExtraFunctions* gl, unsigned type, const char* code);
	~GLShader();

};

struct GLProgram
{
	QOpenGLExtraFunctions* m_gl;
	unsigned m_id = -1;
	GLProgram(QOpenGLExtraFunctions* gl, const GLShader& vertexShader, const GLShader& fragmentShader);
	GLProgram(QOpenGLExtraFunctions* gl, const GLShader& vertexShader, const GLShader& geometryShader, const GLShader& fragmentShader);
	GLProgram(QOpenGLExtraFunctions* gl, const GLShader& computeShader);
	~GLProgram();

};


struct GL2DRGBA
{
	QOpenGLExtraFunctions* m_gl;
	unsigned tex_id;
	GL2DRGBA(QOpenGLExtraFunctions* gl);
	~GL2DRGBA();

	void genMIPs();
	void create(int width, int height);
	void upload(int width, int height, const uint8_t* data);

};

struct GL2DThickness
{
	QOpenGLExtraFunctions* m_gl;
	unsigned tex_id;
	GL2DThickness(QOpenGLExtraFunctions* gl);
	~GL2DThickness();

	void create(int width, int height);
	void upload(int width, int height, const float* data);

};