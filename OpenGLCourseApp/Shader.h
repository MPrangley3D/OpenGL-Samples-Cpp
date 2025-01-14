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
	GLuint GetAmbientColorLocation();
	GLuint GetAmbientIntensityLocation();
	GLuint GetLightDirectionLocation();
	GLuint GetDiffuseIntensityLocation();
	void UseShader();
	void ClearShader();

	~Shader();

private:
	GLuint ShaderID;
	GLuint UniformProjection;
	GLuint UniformView;
	GLuint UniformModel;
	GLuint UniformAmbientIntensity;
	GLuint UniformAmbientColor;
	GLuint UniformDiffuseIntensity;
	GLuint UniformLightDirection;
	void CompileShader(const char* VertexCode, const char* FragmentCode);
	void AddShader(GLuint TheProgram, const char* ShaderCode, GLenum ShaderType);
};

