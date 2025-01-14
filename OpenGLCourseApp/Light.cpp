#include "Light.h"

Light::Light()
{
	Color = glm::vec3(1.0f, 1.0f, 1.0f);
	AmbientIntensity = 1.0f;
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	DiffuseIntensity = 0.0f;
}

Light::Light(GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Intensity,
	GLfloat DirX, GLfloat DirY, GLfloat DirZ, GLfloat NewDiffuseIntensity)
{
	Color = glm::vec3(Red, Green, Blue);
	AmbientIntensity = Intensity;

	Direction = glm::vec3(DirX, DirY, DirZ);
	DiffuseIntensity = NewDiffuseIntensity;
}

// These uniforms pass the values into the bound ID in the shader
void Light::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation,
						GLuint DiffuseIntensityLocation, GLuint DirectionLocation)
{
	glUniform3f(AmbientColorLocation, Color.x, Color.y, Color.z);
	glUniform1f(AmbientIntensityLocation, AmbientIntensity);
	glUniform3f(DirectionLocation, Direction.x, Direction.y, Direction.z);
	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
}

Light::~Light()
{

}
