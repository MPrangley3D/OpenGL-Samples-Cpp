#include "Skybox.h"

Skybox::Skybox()
{
}

Skybox::Skybox(std::vector<std::string> FaceLocations)
{
	// set up Skybox Shader
	SkyShader = new Shader();
	SkyShader->CreateFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag");

	// Set up Uniforms
	UniformProjection = SkyShader->GetProjectionLocation();
	UniformView = SkyShader->GetViewLocation();

	//Texture Setup
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

	int Width;
	int Height;
	int BitDepth;

	for (size_t i = 0; i < 6; i++)
	{
		// &OutParams get set and returned by this STBI function
		unsigned char* TextureData = stbi_load(FaceLocations[i].c_str(), &Width, &Height, &BitDepth, 0);

		if (!TextureData)
		{
			printf("Failed to find a texture at:  %s\n", FaceLocations[i].c_str());
			return;
		}

		// We use UNSIGNED_BYTE here for our texture data's unsigned Chars (Byte & Char are interchangeable here)
		// Sends the Texture Data to our bound TextureID
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);
		stbi_image_free(TextureData);
	}
	// Setup texture parameters for wrapping & filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Mesh Setup Code
	unsigned int SkyboxIndices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	float SkyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	SkyMesh = new Mesh();
	SkyMesh->CreateMesh(SkyboxVertices, SkyboxIndices, 64, 36);
}

void Skybox::DrawSkybox(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix)
{
	printf("Draw Skybox Called!\n");
	// Strip transform data from the View Matrix
	ViewMatrix = glm::mat4(glm::mat3(ViewMatrix));

	// Disable depth checking for Skybox
	glDepthMask(GL_FALSE);

	// Enable Sky Shader
	SkyShader->UseShader();

	// Bind the Uniform Perspective / Projection Matrix
	glUniformMatrix4fv(UniformProjection, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	// Bind the Camera / View Matrix
	glUniformMatrix4fv(UniformView, 1, GL_FALSE, glm::value_ptr(ViewMatrix));

	// Set up the Skybox Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

	// Validate before Rendering
	SkyShader->ValidateShader();

	// Render Mesh
	SkyMesh->RenderMesh();

	// Re-Enable depth checking for rest of scene
	glDepthMask(GL_TRUE);
}

Skybox::~Skybox()
{

}
