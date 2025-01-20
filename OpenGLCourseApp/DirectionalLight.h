#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight();

	DirectionalLight(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
					GLfloat Red, GLfloat Green, GLfloat Blue,
					GLfloat Intensity, GLfloat NewDiffuseIntensity,
					GLfloat DirX, GLfloat DirY, GLfloat DirZ);

	// Bind light values for use in the shader (Location here meaning the ID in the shader, not a position)
	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation,
		GLuint DiffuseIntensityLocation, GLuint DirectionLocation);

	glm::mat4 CalculateLightTransform();

	~DirectionalLight();

private:
	glm::vec3 Direction;
};

