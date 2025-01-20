#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	SpotLight();

	SpotLight(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
		GLfloat NewNearPlane, GLfloat NewFarPlane, 
		GLfloat Red, GLfloat Green, GLfloat Blue,
		GLfloat Intensity, GLfloat NewDiffuseIntensity,
		GLfloat PosX, GLfloat PosY, GLfloat PosZ,
		GLfloat DirX, GLfloat DirY, GLfloat DirZ,
		GLfloat NewConstant, GLfloat NewLinear, GLfloat NewExponent,
		GLfloat NewEdge);

	// Bind light values for use in the shader (Location here meaning the ID in the shader, not a position)
	void UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation,
		GLuint DiffuseIntensityLocation, GLuint PositionLocation, GLuint DirectionLocation,
		GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation,
		GLuint EdgeLocation);

	void SetFlash(glm::vec3 FlashPosition, glm::vec3 FlashDirection);

	~SpotLight();

private:
	glm::vec3 Direction;
	GLfloat Edge;
	GLfloat ProcessedEdge;
};

