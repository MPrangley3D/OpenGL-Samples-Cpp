#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

// Window dimensions
const GLint WIDTH = 800;
const GLint HEIGHT = 600;
const float ToRadians = 3.14159265f / 180.0f;

GLuint VAO;
GLuint VBO;
GLuint ShaderProgram;
GLuint UniformModel;

bool Direction = true;
bool Growing = true;
float TriangleOffset = 0.0f;
float TriangleMaxOffset = 0.7f;
float TriangleIncrement = 0.00005f;
float CurrentAngle = 0.0f;
float CurrentScale = 0.4f;
float MaxScale = 0.9f;
float MinScale = 0.1f;
float ScaleIncrement = 0.0001f;


// Vertex Shader
/*
Version must match our Major and Minor versions as set in GLFW_CONTEXT_VERSION_MAJOR/MINOR
in == input, out == output
layout: This keyword indicates that the following directive is related to the layout of the shader.
location: This directive specifies the location of a vertex attribute.
'= 0': This assigns the location value to be 0.

By using layout (location = 0), we're telling OpenGL that the pos vertex attribute should be read from the 
first (0-indexed) vertex attribute slot when passing data from your application to the shader.

It maps the pos attribute to the first vertex attribute array, which is usually the position attribute. 
This allows OpenGL to correctly bind the vertex data to the corresponding attribute in the shader.
*/

/*
* In OpenGL, the standard convention for vertex attributes is as follows:
Location 0: Position attribute (pos, position, etc.)
Location 1: Color attribute (color, vertexColor, etc.)
Location 2: Texture coordinates attribute (texCoord, uv, etc.)
Location 3: Normal attribute (normal, vertexNormal, etc.)
4-7 | Additional texture coordinates or other custom attributes
*/

static const char* VertexShader = "                                     \n\
#version 330                                                            \n\
                                                                        \n\
layout (location = 0) in vec3 pos;                                      \n\
                                                                        \n\
out vec4 VertexColor;                                                   \n\
                                                                        \n\
                                                                        \n\
uniform mat4 Model;                                                     \n\
                                                                        \n\
                                                                        \n\
void main()                                                             \n\
{                                                                       \n\
    gl_Position = Model * vec4(pos,1.0);                                \n\
    VertexColor = vec4(clamp(pos, 0.0f, 1.0f),1.0f);                    \n\
}";

// Fragment Shader
static const char* FragmentShader = "                               \n\
#version 330                                                        \n\
                                                                    \n\
in vec4 VertexColor;                                                \n\
                                                                    \n\
out vec4 color;                                                     \n\
                                                                    \n\
void main()                                                         \n\
{                                                                   \n\
    color = VertexColor;                                            \n\
}";

void CreateTriangle()
{
    // Hard code triangle vertex values
    GLfloat TriangleVerticies[] = 
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    // "VERTEX SPECIFICATION"
// 1. Generate Vertex Array Object ID
    glGenVertexArrays(1, &VAO);
// 2. Bind VAO to the ID
    glBindVertexArray(VAO);
        
// 3. Generate VBO ID
        glGenBuffers(1, &VBO);
// 4. Bind VBO to ID
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
// 5. Attach vertex data to the bound VBO
            glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVerticies), TriangleVerticies, GL_STATIC_DRAW);
// 6. Define Attribute Pointer
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
// 7. Enable the Attribute Pointer
            glEnableVertexAttribArray(0);
// 8. Unbind the VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);
}

void AddShader(GLuint TheProgram, const char* ShaderCode, GLenum ShaderType)
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
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error compiling the %d Shader Program: '%s'\n", ShaderType, ErrorLog);
        return;
    }

    // Attach shader to the shader program
    glAttachShader(TheProgram, TheShader);
}

void CompileShaders()
{
    // Create the empty Shader Program
    ShaderProgram = glCreateProgram();
    
    if (!ShaderProgram)
    {
        printf("Error creating the Shader Program!");
        return;
    }

    // Add shaders to the program
    AddShader(ShaderProgram, VertexShader, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShader, GL_FRAGMENT_SHADER);

    // Logging errors for the shader
    GLint Result = 0;
    GLchar ErrorLog[1024] = { 0 };

    // Link the shader program
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Result);
    if (!Result)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error linking the Shader Program: '%s'\n", ErrorLog);
        return;
    }

    // Validate Shader Program
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Result);
    if (!Result)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error validating the Shader Program: '%s'\n", ErrorLog);
        return;
    }

    // Get the location of the MoveX Variable from the sahder code in the specified Shader Program
    UniformModel = glGetUniformLocation(ShaderProgram, "Model");

}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        printf("GLFW Initialization failed!");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW Window Properties
    // Open GL Version (3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile = No backwards compatability
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatability
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create & ensure window exists
    GLFWwindow* MainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!MainWindow)
    {
        printf("GLFW Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    int BufferWidth;
    int BufferHeight;
    glfwGetFramebufferSize(MainWindow, &BufferWidth, &BufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(MainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    // Check if GLEW is working
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW Initialization failed!");
        glfwDestroyWindow(MainWindow);
        glfwTerminate();
        return 1;
    }

    // Create viewport & setup size
    glViewport(0, 0, BufferWidth, BufferHeight);

    CreateTriangle();
    CompileShaders();

    // Loop until window closed
    while (!glfwWindowShouldClose(MainWindow))
    {
        // Get + Handle User Input Events
        glfwPollEvents();

        // Update Transform
        if (Direction)
        {
            TriangleOffset += TriangleIncrement;
        }
        else
        {
            TriangleOffset -= TriangleIncrement;
        }

        if (abs(TriangleOffset) >= TriangleMaxOffset)
        {
            Direction = !Direction;
        }

        // Update Rotation
        CurrentAngle += 0.01f;

        // Prevent overflow
        if (CurrentAngle >= 360)
        {
            CurrentAngle -= 360;
        }

        // Update Scale
        if (Growing)
        {
            CurrentScale += ScaleIncrement;
        }
        else
        {
            CurrentScale -= ScaleIncrement;
        }

        if (CurrentScale >= MaxScale)
        {
            Growing = false;
        }
        
        if (CurrentScale <= MinScale)
        {
            Growing = true;
        }

        // Clear window
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Assign the Shader Program
        glUseProgram(ShaderProgram);

        // Defines a 4x4 matrix for the Model Matrix (1.0f) initializes as a Identity Matrix
        glm::mat4 Model(1.0f);

        // Update the Transform by the Model Matrix
        // Note these are applied in reserve order to the object
        Model = glm::translate(Model, glm::vec3(TriangleOffset, TriangleOffset, 0.0f));
        Model = glm::rotate(Model, CurrentAngle * ToRadians, glm::vec3(0.0f, 0.0f, 1.0f));    
        Model = glm::scale(Model, glm::vec3(0.4f, 0.4f, 1.0f));

        // Bind the Uniform Model Matrix
        glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

            // Bind the VAO
            glBindVertexArray(VAO);

                // Draw the Shader
                glDrawArrays(GL_TRIANGLES, 0, 3);

            // Unbind the VAO
            glBindVertexArray(0);

        // Clear the Shader Program
        glUseProgram(0);


        glfwSwapBuffers(MainWindow);
    }

    printf("User closed window.");
    return 0;
}

