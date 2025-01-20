#pragma once

#include <stdio.h>

#include "GL/glew.h"



class ShadowMap
{
public:
	ShadowMap();

	virtual bool Initialize(unsigned int Width, unsigned int Height);
	virtual void Write();
	virtual void Read(GLenum TextureUnit);
	GLuint GetShadowWidth() { return ShadowWidth; }
	GLuint GetShadowHeight() { return ShadowHeight; }

	~ShadowMap();

protected:
	GLuint FrameBufferObject;
	GLuint MyShadowMap;
	GLuint ShadowWidth;
	GLuint ShadowHeight;
};

