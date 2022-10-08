#pragma once

#include <memory>
#include "GLUtils.h"

class DrawTexture
{
public:
	DrawTexture(QOpenGLExtraFunctions* gl, bool premult = false, bool flipY = false);

	void render(unsigned tex_id, int x, int y, int width, int height, bool blending = false);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;
};



