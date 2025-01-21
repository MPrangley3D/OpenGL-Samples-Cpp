#include "Texture.h"
#include "CommonValues.h"

Texture::Texture()
{
	TextureID = 0;
	Width = 0;
	Height = 0;
	BitDepth = 0;
	FilePath = "";
}

Texture::Texture(const char* FilePath) :
	FilePath(FilePath)
{
	TextureID = 0;
	Width = 0;
	Height = 0;
	BitDepth = 0;
	FilePath = FilePath;
}

bool Texture::LoadAlphaTexture()
{
	// &OutParams get set and returned by this STBI function
	unsigned char* TextureData = stbi_load(FilePath, &Width, &Height, &BitDepth, 0);

	if (!TextureData)
	{
		printf("Failed to find a texture at:  %s\n", FilePath);
		return false;
	}

	// Create the Texture ID
	glGenTextures(1, &TextureID);

	// Bind the Texture to the new ID
	glBindTexture(GL_TEXTURE_2D, TextureID);

	// Setup texture parameters for wrapping & filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// We use UNSIGNED_BYTE here for our texture data's unsigned Chars (Byte & Char are interchangeable here)
	// Sends the Texture Data to our bound TextureID
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);

	// Create Mips automatically
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind the Texture from GL_TEXTURE_2D
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clears the loaded data, no longer needed now that it's copied into the TextureID
	stbi_image_free(TextureData);
	return true;
}

bool Texture::LoadTexture()
{
	// &OutParams get set and returned by this STBI function
	unsigned char* TextureData = stbi_load(FilePath, &Width, &Height, &BitDepth, 0);

	if (!TextureData)
	{
		printf("Failed to find a texture at:  %s\n", FilePath);
		return false;
	}

	// Create the Texture ID
	glGenTextures(1, &TextureID);

	// Bind the Texture to the new ID
	glBindTexture(GL_TEXTURE_2D, TextureID);

	// Setup texture parameters for wrapping & filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// We use UNSIGNED_BYTE here for our texture data's unsigned Chars (Byte & Char are interchangeable here)
	// Sends the Texture Data to our bound TextureID
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);

	// Create Mips automatically
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind the Texture from GL_TEXTURE_2D
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clears the loaded data, no longer needed now that it's copied into the TextureID
	stbi_image_free(TextureData);

	return true;
}

void Texture::UseTexture()
{
	// Sets the active "Texture Unit" (Most cards have at least 16, up to 32.
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureID);
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &TextureID);
	TextureID = 0;
	Width = 0;
	Height = 0;
	BitDepth = 0;
	FilePath = "";
}

Texture::~Texture()
{
	ClearTexture();
}