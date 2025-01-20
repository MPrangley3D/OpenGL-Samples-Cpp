#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"


class Shader
{
public:
	Shader();

	void CreateFromString(const char* VertexCode, const char* FragmentCode);
	void CreateFromFiles(const char* VertexPath, const char* FragmentPath);
	void CreateFromFiles(const char* VertexPath, const char* FragmentPath, const char* GeometrytPath);
	void ValidateShader();
	std::string ReadFile(const char* FilePath);
	GLuint GetProjectionLocation();
	GLuint GetViewLocation();
	GLuint GetModelLocation();
	GLuint GetEyePositionLocation();
	GLuint GetAmbientColorLocation();
	GLuint GetAmbientIntensityLocation();
	GLuint GetLightDirectionLocation();
	GLuint GetDiffuseIntensityLocation();
	GLuint GetSpecularIntensityLocation();
	GLuint GetShininessLocation();
	GLuint GetOmniLightPositionLocation();
	GLuint GetFarPlaneLocation();

	void UseShader();
	void ClearShader();
	void SetDirectionalLight(DirectionalLight* MyDirectionalLight);
	void SetPointLights(PointLight* MyPointLights, unsigned int NewLightCount, unsigned int TextureUnit, unsigned int Offset);
	void SetSpotLights(SpotLight* MySpotLights, unsigned int NewLightCount, unsigned int TextureUnit, unsigned int Offset);
	void SetTexture(GLuint TextureUnit);
	void SetDirectionalShadowMap(GLuint TextureUnit);
	void SetDirectionalLightTransform(glm::mat4* LightTransform);
	void SetOmniLightMatrices(std::vector<glm::mat4> InLightMatrices);

	~Shader();

private:
	// Lights
	int PointLightCount;
	int SpotLightCount;

	// Lighting Values - Directional
	struct
	{
		GLuint UniformColor;
		GLuint UniformAmbientIntensity;
		GLuint UniformDiffuseIntensity;

		GLuint UniformDirection;
	} UniformDirectionalLight;

	GLuint UniformPointLightCount;

	// Lighting Values - Point
	struct
	{
		GLuint UniformColor;
		GLuint UniformAmbientIntensity;
		GLuint UniformDiffuseIntensity;

		GLuint UniformPosition;
		GLuint UniformConstant;
		GLuint UniformLinear;
		GLuint UniformExponent;
	} UniformPointLight[MAX_POINT_LIGHTS];

	GLuint UniformSpotLightCount;

	// Lighting Values - Spot
	struct
	{
		GLuint UniformColor;
		GLuint UniformAmbientIntensity;
		GLuint UniformDiffuseIntensity;

		GLuint UniformPosition;
		GLuint UniformConstant;
		GLuint UniformLinear;
		GLuint UniformExponent;

		GLuint UniformDirection;
		GLuint UniformEdge;
	} UniformSpotLight[MAX_SPOT_LIGHTS];

	// World Values
	GLuint ShaderID;
	GLuint UniformProjection;
	GLuint UniformView;
	GLuint UniformModel;
	GLuint UniformEyePosition;

	// Material Values
	GLuint UniformSpecularIntensity;
	GLuint UniformShininess;
	GLuint UniformTexture;

	// Shadow Map Values
	GLuint UniformDirectionalLightTransform;
	GLuint UniformDirectionalShadowMap;
	GLuint UniformOmniLightPosition;
	GLuint UniformFarPlane;
	GLuint UniformLightMatrices[6];

	// Omni Shadow Map
	struct
	{
		GLuint ShadowMapCube;
		GLuint FarPlane;
	} UniformOmniShadowMap[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

	void CompileShader(const char* VertexCode, const char* FragmentCode);
	void CompileShader(const char* VertexCode, const char* FragmentCode, const char* GeometryCode);
	void AddShader(GLuint TheProgram, const char* ShaderCode, GLenum ShaderType);
	void CompileProgram();
};

