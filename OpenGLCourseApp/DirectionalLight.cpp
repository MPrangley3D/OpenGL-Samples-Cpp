#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	Direction = glm::vec3(0.0f, 0.0f, 0.0f);
	LightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

DirectionalLight::DirectionalLight(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
									GLfloat Red, GLfloat Green, GLfloat Blue,
									GLfloat Intensity, GLfloat NewDiffuseIntensity,
									GLfloat DirX, GLfloat DirY, GLfloat DirZ) 
										: Light(NewShadowWidth, NewShadowHeight, Red, Green, Blue, Intensity, NewDiffuseIntensity)
{
	Direction = glm::vec3(DirX, DirY, DirZ);
	LightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
}

DirectionalLight::~DirectionalLight()
{

}

// These uniforms pass the values into the bound ID in the shader
void DirectionalLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation,
								GLuint DiffuseIntensityLocation, GLuint DirectionLocation)
{
	glUniform3f(AmbientColorLocation, Color.x, Color.y, Color.z);
	glUniform1f(AmbientIntensityLocation, AmbientIntensity);
	glUniform3f(DirectionLocation, Direction.x, Direction.y, Direction.z);
	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	return LightProjection * glm::lookAt(-Direction, glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
}
