#pragma once

#include <GL/glew.h>

class Material
{
public:
	Material();
	Material(GLfloat SpecularIntensity, GLfloat Shininess);

	void UseMaterial(GLuint NewSpecularIntensityLocation, GLuint NewShininessLocation);

private:
	GLfloat SpecularIntensity;
	GLfloat Shininess;
};

