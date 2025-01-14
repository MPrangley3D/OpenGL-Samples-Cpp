#include "Shader.h"

Shader::Shader()
{
    ShaderID = 0;
    UniformModel = 0;
    UniformView = 0;
    UniformProjection = 0;
    UniformAmbientIntensity = 0;
    UniformAmbientColor = 0;
    UniformDiffuseIntensity = 0;
    UniformLightDirection = 0;
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

// Uniform Getters
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

GLuint Shader::GetAmbientColorLocation()
{
    return UniformAmbientColor;
}

GLuint Shader::GetAmbientIntensityLocation()
{
    return UniformAmbientIntensity;
}

GLuint Shader::GetLightDirectionLocation()
{
    return UniformLightDirection;
}

GLuint Shader::GetDiffuseIntensityLocation()
{
    return UniformDiffuseIntensity;
}

void Shader::CompileShader(const char* VertexCode, const char* FragmentCode)
{
    // Create the empty Shader Program
    ShaderID = glCreateProgram();

    printf("OpenGL version is (%s)\n", glGetString(GL_VERSION));

    if (!ShaderID)
    {
        printf("Error creating the Shader Program!");
        return;
    }
    else
    {
        printf("Clean shader program created\n");
    }

    // Add shaders to the program
    printf("Add Vertex Shader...\n");
    AddShader(ShaderID, VertexCode, GL_VERTEX_SHADER);

    printf("Add Fragment Shader...\n");
    AddShader(ShaderID, FragmentCode, GL_FRAGMENT_SHADER);

    // Logging errors for the shader
    GLint Result = 0;
    GLchar ErrorLog[1024];

    // Link the shader program
    glLinkProgram(ShaderID);
    glGetProgramiv(ShaderID, GL_LINK_STATUS, &Result);
    if (!Result)
    {
        glGetProgramInfoLog(ShaderID, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error linking the Shader Program: '%s'\n", ErrorLog);
        return;
    }
    else
    {
        printf("Linking Successful!\n");
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
    else
    {
        printf("Validation succeeded!\n");
    }

    // Bind uniform variables to the location of the model in the shader code
    // Note the struct member variable access for the light variables
    UniformModel = glGetUniformLocation(ShaderID, "Model");
    UniformView = glGetUniformLocation(ShaderID, "View");
    UniformProjection = glGetUniformLocation(ShaderID, "Projection");
    UniformAmbientIntensity = glGetUniformLocation(ShaderID, "MyDirectionalLight.AmbientIntensity");
    UniformAmbientColor = glGetUniformLocation(ShaderID, "MyDirectionalLight.Color");
    UniformLightDirection = glGetUniformLocation(ShaderID, "MyDirectionalLight.Direction");
    UniformDiffuseIntensity = glGetUniformLocation(ShaderID, "MyDirectionalLight.DiffuseIntensity");
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
    UniformAmbientIntensity = 0;
    UniformAmbientColor = 0;
    UniformDiffuseIntensity = 0;
    UniformLightDirection = 0;
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
    else
    {
        printf("%d: Compile Suceeded\n", ShaderType);
    }

    // Attach shader to the shader program
    glAttachShader(TheProgram, TheShader);
}

Shader::~Shader()
{
    ClearShader();
}
