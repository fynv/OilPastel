#pragma once

#include <memory>
#include <glm.hpp>
#include "GLUtils.h"

class StrokeDrawTriangles
{
public:
	StrokeDrawTriangles(QOpenGLExtraFunctions* gl);

	void draw(int width, int height, int num_verts, glm::vec2* verts);


private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;
};
