#include <QOpenGLExtraFunctions>
#include "GLUtils.h"


GLBuffer::GLBuffer(QOpenGLExtraFunctions* gl, size_t size, unsigned target) 
	: m_gl(gl)
	, m_target(target)
	, m_size(size)
{
	m_gl->glGenBuffers(1, &m_id);
	m_gl->glBindBuffer(m_target, m_id);
	m_gl->glBufferData(m_target, m_size, nullptr, GL_STATIC_DRAW);
	m_gl->glBindBuffer(m_target, 0);
}

GLBuffer::~GLBuffer()
{
	if (m_id != -1)
		m_gl->glDeleteBuffers(1, &m_id);
}

void GLBuffer::upload(const void* data)
{
	m_gl->glBindBuffer(m_target, m_id);
	m_gl->glBufferData(m_target, m_size, data, GL_STATIC_DRAW);
	m_gl->glBindBuffer(m_target, 0);
}

const GLBuffer& GLBuffer::operator = (const GLBuffer& in)
{
	m_gl->glBindBuffer(GL_COPY_READ_BUFFER, in.m_id);
	m_gl->glBindBuffer(GL_COPY_WRITE_BUFFER, m_id);
	m_gl->glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_size);
	m_gl->glBindBuffer(GL_COPY_READ_BUFFER, 0);
	m_gl->glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	return *this;
}


GLShader::GLShader(QOpenGLExtraFunctions* gl, unsigned type, const char* code)
	: m_gl(gl)
	, m_type(type)
{
	m_id = m_gl->glCreateShader(type);
	m_gl->glShaderSource(m_id, 1, &code, nullptr);
	m_gl->glCompileShader(m_id);

	GLint compileResult;
	m_gl->glGetShaderiv(m_id, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == 0)
	{
		GLint infoLogLength;
		m_gl->glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		m_gl->glGetShaderInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader compilation failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}	
}

GLShader::~GLShader()
{
	if (m_id != -1)
		m_gl->glDeleteShader(m_id);
}


GLProgram::GLProgram(QOpenGLExtraFunctions* gl, const GLShader& vertexShader, const GLShader& fragmentShader) : m_gl(gl)
{
	m_id = m_gl->glCreateProgram();
	m_gl->glAttachShader(m_id, vertexShader.m_id);
	m_gl->glAttachShader(m_id, fragmentShader.m_id);
	m_gl->glLinkProgram(m_id);

	GLint linkResult;
	m_gl->glGetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
	if (linkResult == 0)
	{
		GLint infoLogLength;
		m_gl->glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		m_gl->glGetProgramInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader link failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}

	m_gl->glDetachShader(m_id, vertexShader.m_id);
	m_gl->glDetachShader(m_id, fragmentShader.m_id);
}

GLProgram::GLProgram(QOpenGLExtraFunctions* gl, const GLShader& vertexShader, const GLShader& geometryShader, const GLShader& fragmentShader) : m_gl(gl)
{
	m_id = m_gl->glCreateProgram();
	m_gl->glAttachShader(m_id, vertexShader.m_id);
	m_gl->glAttachShader(m_id, geometryShader.m_id);
	m_gl->glAttachShader(m_id, fragmentShader.m_id);
	m_gl->glLinkProgram(m_id);

	GLint linkResult;
	m_gl->glGetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
	if (linkResult == 0)
	{
		GLint infoLogLength;
		m_gl->glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		m_gl->glGetProgramInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader link failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}

	m_gl->glDetachShader(m_id, vertexShader.m_id);
	m_gl->glDetachShader(m_id, geometryShader.m_id);
	m_gl->glDetachShader(m_id, fragmentShader.m_id);
}

GLProgram::GLProgram(QOpenGLExtraFunctions* gl, const GLShader& computeShader) : m_gl(gl)
{
	m_id = m_gl->glCreateProgram();
	m_gl->glAttachShader(m_id, computeShader.m_id);
	m_gl->glLinkProgram(m_id);

	GLint linkResult;
	m_gl->glGetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
	if (linkResult == 0)
	{
		GLint infoLogLength;
		m_gl->glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		m_gl->glGetProgramInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader link failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}
	m_gl->glDetachShader(m_id, computeShader.m_id);
}

GLProgram::~GLProgram()
{
	if (m_id != -1)
		m_gl->glDeleteProgram(m_id);
}

GL2DRGBA::GL2DRGBA(QOpenGLExtraFunctions* gl) : m_gl(gl)
{
	m_gl->glGenTextures(1, &tex_id);
}

GL2DRGBA::~GL2DRGBA()
{
	m_gl->glDeleteTextures(1, &tex_id);
}

void GL2DRGBA::genMIPs()
{
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glGenerateMipmap(GL_TEXTURE_2D);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
}


void GL2DRGBA::create(int width, int height)
{
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	m_gl->glGenerateMipmap(GL_TEXTURE_2D);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
}

void GL2DRGBA::upload(int width, int height, const uint8_t* data)
{
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	m_gl->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	m_gl->glGenerateMipmap(GL_TEXTURE_2D);
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
}

GL2DThickness::GL2DThickness(QOpenGLExtraFunctions* gl) : m_gl(gl)
{
	m_gl->glGenTextures(1, &tex_id);
}

GL2DThickness::~GL2DThickness()
{
	m_gl->glDeleteTextures(1, &tex_id);
}


void GL2DThickness::create(int width, int height)
{
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);		
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
}


void GL2DThickness::upload(int width, int height, const float* data)
{
	m_gl->glBindTexture(GL_TEXTURE_2D, tex_id);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	m_gl->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);	
	m_gl->glBindTexture(GL_TEXTURE_2D, 0);
}
