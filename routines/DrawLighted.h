#pragma once

#include <glm.hpp>
#include <memory>
#include "Image.h"

class DrawLighted
{
public:
	DrawLighted(QOpenGLExtraFunctions* gl);

	void render(Image& image, int x, int y, int width, int height, const glm::vec3 light_dir);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;
};



