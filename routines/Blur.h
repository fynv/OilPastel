#pragma once

#include <memory>
#include "GLUtils.h"

class BlurRow
{
public:
	BlurRow(QOpenGLExtraFunctions* gl, int width);

	void filter(unsigned tex_id, int height, float filterRadius, float contrast, int numApproxPasses = 3);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;
	int m_width;
	int m_GroupSize = 256;

};

class BlurCol
{
public:
	BlurCol(QOpenGLExtraFunctions* gl, int height);

	void filter(unsigned tex_id, int weight, float filterRadius, float contrast, int numApproxPasses = 3);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;
	int m_height;
	int m_GroupSize = 256;
};