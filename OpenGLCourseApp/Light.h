#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "ShadowMap.h"

class Light
{
public:
	Light();

	Light(GLfloat NewShadowWidth, GLfloat NewShadowHeight,
			GLfloat Red, GLfloat Green, GLfloat Blue, 
			GLfloat Intensity, GLfloat NewDiffuseIntensity);

	ShadowMap* GetShadowMap() { return MyShadowMap; }

	~Light();

protected:
	glm::vec3 Color;
	GLfloat AmbientIntensity;
	GLfloat DiffuseIntensity;

	glm::mat4 LightProjection;

	ShadowMap* MyShadowMap;
};

