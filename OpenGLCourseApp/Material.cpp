#include "Material.h"

Material::Material()
{
	SpecularIntensity = 0.0f;
	Shininess = 0.0f;
}

Material::Material(GLfloat NewSpecularIntensity, GLfloat NewShininess)
{
	SpecularIntensity = NewSpecularIntensity;
	Shininess = NewShininess;
}

void Material::UseMaterial(GLuint SpecularIntensityLocation, GLuint ShininessLocation)
{
	glUniform1f(SpecularIntensityLocation, SpecularIntensity);
	glUniform1f(ShininessLocation, Shininess);
}