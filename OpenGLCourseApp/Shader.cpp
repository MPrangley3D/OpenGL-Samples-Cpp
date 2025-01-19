#include "Shader.h"

Shader::Shader()
{
    ShaderID = 0;
    UniformModel = 0;
    UniformView = 0;
    UniformProjection = 0;
    UniformEyePosition = 0;
    UniformDirectionalLight.UniformAmbientIntensity = 0;
    UniformDirectionalLight.UniformColor = 0;
    UniformDirectionalLight.UniformDirection = 0;
    UniformDirectionalLight.UniformDiffuseIntensity = 0;
    UniformSpecularIntensity = 0;
    UniformShininess = 0;
    PointLightCount = 0;
    UniformPointLightCount = 0;
    UniformSpotLightCount = 0;
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

// Getters for Uniform Variables
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

GLuint Shader::GetEyePositionLocation()
{
    return UniformEyePosition;
}

GLuint Shader::GetAmbientColorLocation()
{
    return UniformDirectionalLight.UniformColor;
}

GLuint Shader::GetAmbientIntensityLocation()
{
    return UniformDirectionalLight.UniformAmbientIntensity;
}

GLuint Shader::GetLightDirectionLocation()
{
    return UniformDirectionalLight.UniformDirection;
}

GLuint Shader::GetDiffuseIntensityLocation()
{
    return UniformDirectionalLight.UniformDiffuseIntensity;
}

GLuint Shader::GetSpecularIntensityLocation()
{
    return UniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation()
{
    return UniformShininess;
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

    // Bind uniforms for Material Uses
    UniformEyePosition = glGetUniformLocation(ShaderID, "EyePosition");
    UniformSpecularIntensity = glGetUniformLocation(ShaderID, "MyMaterial.SpecularIntensity");
    UniformShininess = glGetUniformLocation(ShaderID, "MyMaterial.Shininess");

    // Bind uniforms for Directional Light
    UniformDirectionalLight.UniformAmbientIntensity = glGetUniformLocation(ShaderID, "MyDirectionalLight.Base.AmbientIntensity");
    UniformDirectionalLight.UniformColor = glGetUniformLocation(ShaderID, "MyDirectionalLight.Base.Color");
    UniformDirectionalLight.UniformDiffuseIntensity = glGetUniformLocation(ShaderID, "MyDirectionalLight.Base.DiffuseIntensity");
    UniformDirectionalLight.UniformDirection = glGetUniformLocation(ShaderID, "MyDirectionalLight.Direction");

    // Bind uniforms for Point Lights
    UniformPointLightCount = glGetUniformLocation(ShaderID, "PointLightCount");

    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        char LocationBuffer[100] = { '\0' };

        // Bind point light Color Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Base.Color", i);
        UniformPointLight[i].UniformColor = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind point light Ambient Intensity Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Base.AmbientIntensity", i);
        UniformPointLight[i].UniformAmbientIntensity = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind point light Diffuse Intensity Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Base.DiffuseIntensity", i);
        UniformPointLight[i].UniformDiffuseIntensity = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind point light Position Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Position", i);
        UniformPointLight[i].UniformPosition = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind point light Constant Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Constant", i);
        UniformPointLight[i].UniformConstant = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind point light Linear Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Linear", i);
        UniformPointLight[i].UniformLinear = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind point light Exponent Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MyPointLights[%d].Exponent", i);
        UniformPointLight[i].UniformExponent = glGetUniformLocation(ShaderID, LocationBuffer);
    }

    // Bind uniforms for Spot Lights
    UniformSpotLightCount = glGetUniformLocation(ShaderID, "SpotLightCount");
    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        char LocationBuffer[100] = { '\0' };

        // Bind spot light Color Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Base.Color", i);
        UniformSpotLight[i].UniformColor = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Ambient Intensity Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Base.AmbientIntensity", i);
        UniformSpotLight[i].UniformAmbientIntensity = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Diffuse Intensity Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Base.DiffuseIntensity", i);
        UniformSpotLight[i].UniformDiffuseIntensity = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Position Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Position", i);
        UniformSpotLight[i].UniformPosition = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Constant Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Constant", i);
        UniformSpotLight[i].UniformConstant = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Linear Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Linear", i);
        UniformSpotLight[i].UniformLinear = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Exponent Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Base.Exponent", i);
        UniformSpotLight[i].UniformExponent = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Direction Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Direction", i);
        UniformSpotLight[i].UniformDirection = glGetUniformLocation(ShaderID, LocationBuffer);

        // Bind spot light Edge Uniform
        snprintf(LocationBuffer, sizeof(LocationBuffer), "MySpotLights[%d].Edge", i);
        UniformSpotLight[i].UniformEdge = glGetUniformLocation(ShaderID, LocationBuffer);
    }
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
    UniformView = 0;
    UniformProjection = 0;
    UniformEyePosition = 0;
    UniformDirectionalLight.UniformAmbientIntensity = 0;
    UniformDirectionalLight.UniformColor = 0;
    UniformDirectionalLight.UniformDirection = 0;
    UniformDirectionalLight.UniformDiffuseIntensity = 0;
    UniformSpecularIntensity = 0;
    UniformShininess = 0;
}

void Shader::SetDirectionalLight(DirectionalLight* MyDirectionalLight)
{
    /*      Expected signature:
    	void UseLight(GLuint AmbientIntensityLocation, 
                        GLuint AmbientColorLocation,
		                GLuint DiffuseIntensityLocation, 
                        GLuint DirectionLocation);
    */
    MyDirectionalLight->UseLight(UniformDirectionalLight.UniformAmbientIntensity, 
                                    UniformDirectionalLight.UniformColor,
                                    UniformDirectionalLight.UniformDiffuseIntensity, 
                                    UniformDirectionalLight.UniformDirection);
}

void Shader::SetPointLights(PointLight* MyPointLights, unsigned int NewLightCount)
{
    if (NewLightCount > MAX_POINT_LIGHTS)
    {
        NewLightCount = MAX_POINT_LIGHTS;
    }

    glUniform1i(UniformPointLightCount, NewLightCount);

    for (size_t i = 0; i < NewLightCount; i++)
    {
        /*      Expected signature:
        * 	void UseLight(GLuint AmbientIntensityLocation, 
        *                   GLuint AmbientColorLocation,
		*			        GLuint DiffuseIntensityLocation, 
        *                   GLuint PositionLocation,
		*			        GLfloat ConstantLocation, 
        *                   GLfloat LinearLocation, 
        *                   GLfloat ExponentLocation);
        */
        MyPointLights[i].UseLight(UniformPointLight[i].UniformAmbientIntensity,
            UniformPointLight[i].UniformColor,
            UniformPointLight[i].UniformDiffuseIntensity,
            UniformPointLight[i].UniformPosition,
            UniformPointLight[i].UniformConstant,
            UniformPointLight[i].UniformLinear,
            UniformPointLight[i].UniformExponent);
    }
}

void Shader::SetSpotLights(SpotLight* MySpotLights, unsigned int NewLightCount)
{
    if (NewLightCount > MAX_SPOT_LIGHTS)
    {
        NewLightCount = MAX_SPOT_LIGHTS;
    }

    glUniform1i(UniformSpotLightCount, NewLightCount);

    for (size_t i = 0; i < NewLightCount; i++)
    {
        /*      Expected signature:
        * 	void UseLight(GLuint AmbientIntensityLocation, 
                            GLuint AmbientColorLocation,
		                    GLuint DiffuseIntensityLocation, 
                            GLuint PositionLocation, 
                            GLuint DirectionLocation,
		                    GLuint ConstantLocation, 
                            GLuint LinearLocation, 
                            GLuint ExponentLocation,
		                    GLuint EdgeLocation);
        */
        MySpotLights[i].UseLight(UniformSpotLight[i].UniformAmbientIntensity,
            UniformSpotLight[i].UniformColor,
            UniformSpotLight[i].UniformDiffuseIntensity,
            UniformSpotLight[i].UniformPosition,
            UniformSpotLight[i].UniformDirection,
            UniformSpotLight[i].UniformConstant,
            UniformSpotLight[i].UniformLinear,
            UniformSpotLight[i].UniformExponent,
            UniformSpotLight[i].UniformEdge);
    }
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
