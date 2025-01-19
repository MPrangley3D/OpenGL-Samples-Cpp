#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"


class Shader
{
public:
	Shader();

	void CreateFromString(const char* VertexCode, const char* FragmentCode);
	void CreateFromFiles(const char* VertexPath, const char* FragmentPath);
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
	void UseShader();
	void ClearShader();
	void SetDirectionalLight(DirectionalLight* MyDirectionalLight);
	void SetPointLights(PointLight* MyPointLights, unsigned int PointLightCount);

	~Shader();

private:
	// Lights
	int PointLightCount;

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

	// World Values
	GLuint ShaderID;
	GLuint UniformProjection;
	GLuint UniformView;
	GLuint UniformModel;
	GLuint UniformEyePosition;

	// Material Values
	GLuint UniformSpecularIntensity;
	GLuint UniformShininess;

	void CompileShader(const char* VertexCode, const char* FragmentCode);
	void AddShader(GLuint TheProgram, const char* ShaderCode, GLenum ShaderType);
};

