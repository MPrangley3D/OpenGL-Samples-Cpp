#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Intensity, 
		GLfloat DirX, GLfloat DirY, GLfloat DirZ, GLfloat NewDiffuseIntensity);

	// Bind light values for use in the shader (Location here meaning the ID in the shader, not a position)
	void UseLight(GLfloat AmbientIntensityLocation, GLfloat AmbientColorLocation, 
					GLfloat DiffuseIntensityLocation, GLfloat DirectionLocation);

	~Light();

private:
	glm::vec3 Color;
	GLfloat AmbientIntensity;

	glm::vec3 Direction;
	GLfloat DiffuseIntensity;
};

