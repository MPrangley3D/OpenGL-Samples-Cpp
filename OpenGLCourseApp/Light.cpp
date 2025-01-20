#include "Light.h"

Light::Light()
{
	Color = glm::vec3(1.0f, 1.0f, 1.0f);
	AmbientIntensity = 1.0f;
	DiffuseIntensity = 0.0f;
}

Light::Light(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
	GLfloat Red, GLfloat Green, GLfloat Blue,
	GLfloat Intensity, GLfloat NewDiffuseIntensity)
{
	MyShadowMap = new ShadowMap();
	MyShadowMap->Initialize(NewShadowWidth, NewShadowHeight);
	Color = glm::vec3(Red, Green, Blue);
	AmbientIntensity = Intensity;
	DiffuseIntensity = NewDiffuseIntensity;
}

Light::~Light()
{

}
