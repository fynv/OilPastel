#pragma once

#include <memory>
#include "GLUtils.h"

class CompNorm
{
public:
	CompNorm(QOpenGLExtraFunctions* gl);

	void comp(unsigned tex_id_thickness, unsigned tex_id_norm, int width, int height, float pix_size);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;

};

