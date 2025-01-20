#include "PointLight.h"

PointLight::PointLight() : Light()
{
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Constant = 1.0f;  // Ensures we don't divide by 0
	Linear = 0.0f;
	Exponent = 0.0f;
}

PointLight::PointLight(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
						GLfloat NewNearPlane, GLfloat NewFarPlane,
						GLfloat Red, GLfloat Green, GLfloat Blue,
						GLfloat Intensity, GLfloat NewDiffuseIntensity,
						GLfloat PosX, GLfloat PosY, GLfloat PosZ,
						GLfloat NewConstant, GLfloat NewLinear, GLfloat NewExponent) 
							: Light(NewShadowWidth, NewShadowHeight, Red,Green,Blue,Intensity,NewDiffuseIntensity)
{
	Position = glm::vec3(PosX, PosY, PosZ);
	Constant = NewConstant;
	Linear = NewLinear;
	Exponent = NewExponent;
	FarPlane = NewFarPlane;
	NearPlane = NewNearPlane;

	// Set up Projection Matrix
	float AspectRatio = NewShadowWidth / NewShadowHeight;
	LightProjection = glm::perspective(glm::radians(90.0f), AspectRatio, NearPlane, FarPlane);

	// Initialize Omni Shadowmap
	MyShadowMap = new OmniShadowMap();
	MyShadowMap->Initialize(NewShadowWidth, NewShadowHeight);
}

void PointLight::UseLight(GLuint AmbientIntensityLocation, GLuint AmbientColorLocation, 
							GLuint DiffuseIntensityLocation, GLuint PositionLocation, 
							GLuint ConstantLocation, GLuint LinearLocation, GLuint ExponentLocation)
{
	glUniform3f(AmbientColorLocation, Color.x, Color.y, Color.z);
	glUniform1f(AmbientIntensityLocation, AmbientIntensity);
	glUniform1f(DiffuseIntensityLocation, DiffuseIntensity);
	glUniform3f(PositionLocation, Position.x, Position.y, Position.z);
	glUniform1f(ConstantLocation, Constant);
	glUniform1f(LinearLocation, Linear);
	glUniform1f(ExponentLocation, Exponent);
}

std::vector<glm::mat4> PointLight::CalculateLightTransforms()
{
	std::vector<glm::mat4> LightMatrices;
	// Define views in specific order: PosX, NegX, PosY, NegY, PosZ, NegZ
	// Positive X
	LightMatrices.push_back(LightProjection * glm::lookAt(Position, Position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	// Negative X
	LightMatrices.push_back(LightProjection * glm::lookAt(Position, Position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));

	// Positive Y
	LightMatrices.push_back(LightProjection * glm::lookAt(Position, Position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	// Negative Y
	LightMatrices.push_back(LightProjection * glm::lookAt(Position, Position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));

	// Positive Z
	LightMatrices.push_back(LightProjection * glm::lookAt(Position, Position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	// Negative Z
	LightMatrices.push_back(LightProjection * glm::lookAt(Position, Position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
		
	return LightMatrices;
}

PointLight::~PointLight()
{
}
