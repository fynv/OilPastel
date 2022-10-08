#pragma once

#include <memory>
#include "GLUtils.h"

#include "Image.h"

class BlitImage
{
public:
	BlitImage(QOpenGLExtraFunctions* gl);

	void Copy(Image& img_out, Image& img_in);

private:
	QOpenGLExtraFunctions* m_gl;
	std::unique_ptr<GLProgram> m_prog;

};

