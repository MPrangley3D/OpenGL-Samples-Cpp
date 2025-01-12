#pragma once

#include <GL/glew.h>

#include "stb_image.h"

class Texture
{

public:
	Texture();
	Texture(char* FilePath);
	~Texture();

	void LoadTexture();
	void UseTexture();
	void ClearTexture();

private:
	GLuint TextureID;
	int Width;
	int Height;
	int BitDepth;
	char* FilePath;
};

