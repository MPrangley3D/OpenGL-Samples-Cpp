#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	FrameBufferObject = 0;
	MyShadowMap = 0;
	ShadowWidth = 0;
	ShadowHeight = 0;
}

bool ShadowMap::Initialize(GLuint Width, GLuint Height)
{
	ShadowWidth = Width;
	ShadowHeight = Height;

	// Set up ShadowMap Framebuffer
	glGenFramebuffers(1, &FrameBufferObject);
	glGenTextures(1, &MyShadowMap);

	// Set up Depth Map Texture
	glBindTexture(GL_TEXTURE_2D, MyShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	
	// Setup texture parameters for wrapping & filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObject);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, MyShadowMap, 0);

	// Frame buffer doesn't draw or read from Color attachments (Greyscale map)
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status == GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Depth Initialize Success!\n");
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

void ShadowMap::Write()
{
	// Bind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObject);
}

void ShadowMap::Read(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, MyShadowMap);
}

ShadowMap::~ShadowMap()
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
