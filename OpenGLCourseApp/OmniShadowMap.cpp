#include "OmniShadowMap.h"

OmniShadowMap::OmniShadowMap() : ShadowMap() {}

bool OmniShadowMap::Initialize(unsigned int Width, unsigned int Height)
{
	ShadowWidth = Width;
	ShadowHeight = Height;

	// Set up ShadowMap Framebuffer
	glGenFramebuffers(1, &FrameBufferObject);
	glGenTextures(1, &MyShadowMap);

	// Set up Depth Map Cubemap
	glBindTexture(GL_TEXTURE_CUBE_MAP, MyShadowMap);

	// Set up all 6 textures for the cube map faces
	for (size_t i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	// Set up texture parameters filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Setup texture parameters for wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Bind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObject);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, MyShadowMap, 0);

	// Frame buffer doesn't draw or read from Color attachments (Greyscale map)
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status == GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Cube Map Framebuffer Depth Initialize Success!\n");
	}
	else
	{
		printf("Framebuffer Error:  %i\n", Status);
		return false;
	}

	// Unbind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void OmniShadowMap::Write()
{
	// Bind Framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FrameBufferObject);
}

void OmniShadowMap::Read(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, MyShadowMap);
}

OmniShadowMap::~OmniShadowMap()
{
	if (FrameBufferObject)
	{
		glDeleteFramebuffers(1, &FrameBufferObject);
	}

	if (MyShadowMap)
	{
		glDeleteTextures(1, &MyShadowMap);
	}
}
