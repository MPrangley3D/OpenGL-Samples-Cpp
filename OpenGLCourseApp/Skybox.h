#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Shader.h"
#include "Mesh.h"

class Skybox
{
public:
	Skybox();
	Skybox(std::vector<std::string> FaceLocations);

	void DrawSkybox(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix);

	~Skybox();

private:
	Mesh* SkyMesh;
	Shader* SkyShader;

	GLuint TextureID;
	GLuint UniformProjection;
	GLuint UniformView;

};