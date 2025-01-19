#pragma once

#include <GL/glew.h>

#include "stb_image.h"

class Texture
{

public:
	Texture();
	Texture(const char* FilePath);
	~Texture();

	bool LoadTexture();
	bool LoadAlphaTexture();
	void UseTexture();
	void ClearTexture();

private:
	GLuint TextureID;
	int Width;
	int Height;
	int BitDepth;
	const char* FilePath;
};

