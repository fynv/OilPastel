#pragma once

#include <memory>
#include "GLUtils.h"
#include "Image.h"
#include "Stroke.h"


class StrokeBlend
{
public:
	StrokeBlend(QOpenGLExtraFunctions* gl);

	void Blend(Image& img, Stroke& stroke);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;

};