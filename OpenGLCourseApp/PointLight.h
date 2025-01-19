#pragma once
#include "Light.h"
class PointLight : public Light
{
public:
	PointLight();

	PointLight(GLfloat Red, GLfloat Green, GLfloat Blue, 
				GLfloat Intensity, GLfloat NewDiffuseIntensity,
				GLfloat PosX, GLfloat PosY, GLfloat PosZ,
				GLfloat NewConstant, GLfloat NewLinear, GLfloat NewExponent);

	// Bind light values for use in the shader (Location here meaning the ID in the shader, not a position)
	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation,
					GLuint DiffuseIntensityLocation, GLuint PositionLocation,
					GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation);

	~PointLight();

protected:
	glm::vec3 Position;

	// Attenuation values
	// ax^2 + bx + c
	GLfloat Constant;
	GLfloat Linear;
	GLfloat Exponent;
};

