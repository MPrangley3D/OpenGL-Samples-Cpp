#pragma once

#include <stdio.h>

#include "GL/glew.h"



class ShadowMap
{
public:
	ShadowMap();

	virtual bool Initialize(GLuint Width, GLuint Height);
	virtual void Write();
	virtual void Read(GLenum TextureUnit);
	GLuint GetShadowWidth() { return ShadowWidth; }
	GLuint GetShadowHeight() { return ShadowHeight; }

	~ShadowMap();

private:
	GLuint FrameBufferObject;
	GLuint MyShadowMap;
	GLuint ShadowWidth;
	GLuint ShadowHeight;
};

