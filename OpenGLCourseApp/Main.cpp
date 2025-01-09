#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

// Window dimensions
const GLint WIDTH = 800;
const GLint HEIGHT = 600;
GLuint VAO;
GLuint VBO;
GLuint ShaderProgram;
GLuint UniformMoveX;

bool Direction = true;
float TriangleOffset = 0.0f;
float TriangleMaxOffset = 0.7f;
float TriangleIncrement = 0.00005f;


// Vertex Shader
static const char* VertexShader = "                                     \n\
#version 330                                                            \n\
                                                                        \n\
layout (location = 0) in vec3 pos;                                      \n\
                                                                        \n\
    uniform float MoveX;                                                \n\
                                                                        \n\
                                                                        \n\
void main()                                                             \n\
{                                                                       \n\
    gl_Position = vec4(0.4 * pos.x + MoveX, 0.4 * pos.y, pos.z, 1.0);   \n\
}";

// Fragment Shader
static const char* FragmentShader = "                               \n\
#version 330                                                        \n\
                                                                    \n\
out vec4 color;                                                     \n\
                                                                    \n\
void main()                                                         \n\
{                                                                   \n\
    color = vec4(0.0, 1.0, 0.0, 1.0);                               \n\
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
    UniformMoveX = glGetUniformLocation(ShaderProgram, "MoveX");

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

        // Clear window
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Assign the Shader Program
        glUseProgram(ShaderProgram);

            // Bind the Uniform Variable
        glUniform1f(UniformMoveX, TriangleOffset);

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

