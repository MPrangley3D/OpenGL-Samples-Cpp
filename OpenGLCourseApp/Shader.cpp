#include "Shader.h"

Shader::Shader()
{
    ShaderID = 0;
    UniformModel = 0;
    UniformView = 0;
    UniformProjection = 0;
}

void Shader::CreateFromString(const char* VertexCode, const char* FragmentCode)
{
    CompileShader(VertexCode, FragmentCode);
}

void Shader::CreateFromFiles(const char* VertexPath, const char* FragmentPath)
{
    std::string VertexString = ReadFile(VertexPath);
    std::string FragmentString = ReadFile(FragmentPath);
    const char* VertexCode = VertexString.c_str();
    const char* FragmentCode = FragmentString.c_str();
    CompileShader(VertexCode, FragmentCode);
}

std::string Shader::ReadFile(const char* FilePath)
{
    std::string Content;
    std::ifstream Filestream(FilePath, std::ios::in);

    if (!Filestream.is_open())
    {
        printf("Failed to read %s! File doesn't exist...", FilePath);
    }

    std::string Line = "";

    while (!Filestream.eof())
    {
        std::getline(Filestream, Line);
        Content.append(Line + "\n");
    }

    Filestream.close();

    return Content;
}

GLuint Shader::GetProjectionLocation()
{
    return UniformProjection;
}

GLuint Shader::GetViewLocation()
{
    return UniformView;
}

GLuint Shader::GetModelLocation()
{
    return UniformModel;
}

void Shader::CompileShader(const char* VertexCode, const char* FragmentCode)
{
    // Create the empty Shader Program
    ShaderID = glCreateProgram();

    if (!ShaderID)
    {
        printf("Error creating the Shader Program!");
        return;
    }

    // Add shaders to the program
    AddShader(ShaderID, VertexCode, GL_VERTEX_SHADER);
    AddShader(ShaderID, FragmentCode, GL_FRAGMENT_SHADER);

    // Logging errors for the shader
    GLint Result = 0;
    GLchar ErrorLog[1024] = { 0 };

    // Link the shader program
    glLinkProgram(ShaderID);
    glGetProgramiv(ShaderID, GL_LINK_STATUS, &Result);
    if (!Result)
    {
        glGetProgramInfoLog(ShaderID, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error linking the Shader Program: '%s'\n", ErrorLog);
        return;
    }

    // Validate Shader Program
    glValidateProgram(ShaderID);
    glGetProgramiv(ShaderID, GL_VALIDATE_STATUS, &Result);
    if (!Result)
    {
        glGetProgramInfoLog(ShaderID, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error validating the Shader Program: '%s'\n", ErrorLog);
        return;
    }

    // Bind uniform variables to the location of the model in the shader code
    UniformModel = glGetUniformLocation(ShaderID, "Model");
    UniformView = glGetUniformLocation(ShaderID, "View");
    UniformProjection = glGetUniformLocation(ShaderID, "Projection");
}

void Shader::UseShader()
{
    glUseProgram(ShaderID);
}

void Shader::ClearShader()
{
    if (ShaderID != 0)
    {
        glDeleteProgram(ShaderID);
        ShaderID = 0;
    }

    UniformModel = 0;
    UniformProjection = 0;
    UniformView = 0;
}

void Shader::AddShader(GLuint TheProgram, const char* ShaderCode, GLenum ShaderType)
{
    // Create a new shader of the specified type
    GLuint TheShader = glCreateShader(ShaderType);

    // Store the input code
    const GLchar* TheCode[1];
    TheCode[0] = ShaderCode;

    // Get the length of the code file
    GLint CodeLength[1];
    CodeLength[0] = strlen(ShaderCode);

    // Set the shader code on the GPU
    glShaderSource(TheShader, 1, TheCode, CodeLength);

    // Compile the shader
    glCompileShader(TheShader);

    // Logging errors for the shader
    GLint Result = 0;
    GLchar ErrorLog[1024] = { 0 };

    glGetShaderiv(TheShader, GL_COMPILE_STATUS, &Result);
    if (!Result)
    {
        glGetProgramInfoLog(ShaderID, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error compiling the %d Shader Program: '%s'\n", ShaderType, ErrorLog);
        return;
    }

    // Attach shader to the shader program
    glAttachShader(TheProgram, TheShader);
}

Shader::~Shader()
{
    ClearShader();
}
