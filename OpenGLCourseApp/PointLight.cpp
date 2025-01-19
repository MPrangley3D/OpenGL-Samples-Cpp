#include "PointLight.h"

PointLight::PointLight()
{
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Constant = 1.0f;  // Ensures we don't divide by 0
	Linear = 0.0f;
	Exponent = 0.0f;
}

PointLight::PointLight(GLfloat Red, GLfloat Green, GLfloat Blue, 
						GLfloat Intensity, GLfloat NewDiffuseIntensity, 
						GLfloat PosX, GLfloat PosY, GLfloat PosZ, 
						GLfloat NewConstant, GLfloat NewLinear, GLfloat NewExponent) : Light(Red,Green,Blue,Intensity,NewDiffuseIntensity)
{
	Position = glm::vec3(PosX, PosY, PosZ);
	Constant = NewConstant;
	Linear = NewLinear;
	Exponent = NewExponent;
}

void PointLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation, 
							GLuint DiffuseIntensityLocation, GLuint PositionLocation, 
							GLfloat ConstantLocation, GLfloat LinearLocation, GLfloat ExponentLocation)
{
	glUniform3f(AmbientColorLocation, Color.x, Color.y, Color.z);
	glUniform1f(AmbientIntensityLocation, AmbientIntensity);
	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
	glUniform3f(PositionLocation, Position.x, Position.y, Position.z);
	glUniform1f(ConstantLocation, Constant);
	glUniform1f(LinearLocation, Linear);
	glUniform1f(ExponentLocation, Exponent);
}

PointLight::~PointLight()
{
}
