#pragma once

#include <memory>
#include "GLUtils.h"

class DrawThickness
{
public:
	DrawThickness(QOpenGLExtraFunctions* gl, bool flipY = false);

	void render(unsigned tex_id, int x, int y, int width, int height);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;
};



