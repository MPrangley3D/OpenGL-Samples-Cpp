#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

class Light
{
public:
	Light();

	Light(GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Intensity, GLfloat NewDiffuseIntensity);

	~Light();

protected:
	glm::vec3 Color;
	GLfloat AmbientIntensity;
	GLfloat DiffuseIntensity;
};

