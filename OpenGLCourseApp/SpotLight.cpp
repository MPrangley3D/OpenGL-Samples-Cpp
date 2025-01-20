#include "SpotLight.h"

SpotLight::SpotLight() : PointLight()
{
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	Edge = 0.0f;
	ProcessedEdge = cosf(glm::radians(Edge));
	bEnableFlashlight = true;
}

SpotLight::SpotLight(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
	GLfloat NewNearPlane, GLfloat NewFarPlane, 
	GLfloat Red, GLfloat Green, GLfloat Blue,
	GLfloat Intensity, GLfloat NewDiffuseIntensity,
	GLfloat PosX, GLfloat PosY, GLfloat PosZ,
	GLfloat DirX, GLfloat DirY, GLfloat DirZ,
	GLfloat NewConstant, GLfloat NewLinear, GLfloat NewExponent,
	GLfloat NewEdge) : PointLight(NewShadowWidth, NewShadowHeight, NewNearPlane, NewFarPlane, Red, Green, Blue, Intensity, NewDiffuseIntensity, PosX, PosY, PosZ, NewConstant, NewLinear, NewExponent)
{
	Direction = glm::normalize(glm::vec3(DirX, DirY, DirZ));
	Edge = NewEdge;
	ProcessedEdge = cosf(glm::radians(Edge));
}

void SpotLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation,
	GLuint DiffuseIntensityLocation, GLuint PositionLocation, GLuint DirectionLocation,
	GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation,
	GLuint EdgeLocation)
{
	glUniform3f(AmbientColorLocation, Color.x, Color.y, Color.z);
	if (bEnableFlashlight)
	{
		glUniform1f(AmbientIntensityLocation, AmbientIntensity);
		glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
	}
	else
	{
		glUniform1f(AmbientIntensityLocation, 0.0f);
		glUniform1f(DiffuseIntensityLocation, 0.0f);
	}
	glUniform3f(PositionLocation, Position.x, Position.y, Position.z);
	glUniform1f(ConstantLocation, Constant);
	glUniform1f(LinearLocation, Linear);
	glUniform1f(ExponentLocation, Exponent);
	glUniform3f(DirectionLocation, Direction.x, Direction.y, Direction.z);
	glUniform1f(EdgeLocation, ProcessedEdge);
}

void SpotLight::SetFlash(glm::vec3 FlashPosition, glm::vec3 FlashDirection)
{
	Position = FlashPosition;
	Direction = FlashDirection;
}

void SpotLight::ToggleSpotlight(bool NewSetting)
{
	bEnableFlashlight = NewSetting;
}

SpotLight::~SpotLight()
{

}
