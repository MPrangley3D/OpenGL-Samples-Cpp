#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

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

	~Shader();

private:
	// World Values
	GLuint ShaderID;
	GLuint UniformProjection;
	GLuint UniformView;
	GLuint UniformModel;
	GLuint UniformEyePosition;

	// Lighting Values
	GLuint UniformAmbientIntensity;
	GLuint UniformAmbientColor;
	GLuint UniformDiffuseIntensity;
	GLuint UniformLightDirection;

	// Material Values
	GLuint UniformSpecularIntensity;
	GLuint UniformShininess;

	void CompileShader(const char* VertexCode, const char* FragmentCode);
	void AddShader(GLuint TheProgram, const char* ShaderCode, GLenum ShaderType);
};

