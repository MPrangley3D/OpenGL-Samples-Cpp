#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"

GLWindow MainWindow;
std::vector<Mesh*> Meshes;
std::vector<Shader> Shaders;

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
 Modern cards support up to 16 locations
*/

static const char* VertexShader = "Shaders/shader.vert";
static const char* FragmentShader = "Shaders/shader.frag";

void CreateObjects()
{
    //Specify the point sets that make our pyramid
    unsigned int Indicies[] =
    {
        0,3,1,
        1,3,2,
        2,3,0,
        0,1,2
    };

    GLfloat GeometryVerticies[] = 
    {
          0.0f,  0.0f, 1.0f, //0
         -1.0f, -1.0f, 0.0f, //1
          1.0f, -1.0f, 0.0f, //2
          0.0f,  1.0f, 0.0f  //3
    };

    Mesh* Object1 = new Mesh();
    Object1->CreateMesh(GeometryVerticies, Indicies, 12, 12);
    Meshes.push_back(Object1);

    Mesh* Object2 = new Mesh();
    Object2->CreateMesh(GeometryVerticies, Indicies, 12, 12);
    Meshes.push_back(Object2);
}

void CreateShaders()
{
    Shader* Shader1 = new Shader();
    Shader1->CreateFromFiles(VertexShader, FragmentShader);
    Shaders.push_back(*Shader1);
}

int main()
{
    MainWindow = GLWindow(800, 600);
    MainWindow.Initialize();

    CreateObjects();
    CreateShaders();

    GLuint UniformProjection = 0;
    GLuint UniformView = 0;
    GLuint UniformModel = 0;

    // We only need to set up Projection once, so we do it here rather than in the While loop
    glm::mat4 Projection = glm::perspective(45.0f, MainWindow.GetBufferWidth() / MainWindow.GetBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while (!MainWindow.GetShouldCloseWindow())
    {
        // Get + Handle User Input Events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f,0.0f,0.0f,1.0f);

        // Clear the color & depth buffer bits
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Assign the Shader Program
        Shaders[0].UseShader();
        UniformModel = Shaders[0].GetModelLocation();
        UniformProjection = Shaders[0].GetProjectionLocation();

        // Defines a 4x4 matrix for the Model Matrix (1.0f) initializes as a Identity Matrix
        glm::mat4 Model(1.0f);

        // Update the Transform by the Model Matrix
        // Note these are applied in reserve order to the object
        Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -2.5f));
        Model = glm::rotate(Model, 0.0f, glm::vec3(0.5f, 0.5f, 0.5f)); 
        Model = glm::scale(Model, glm::vec3(0.25f, 0.25f, 0.25f));

        // Bind the Uniform Model Matrix
        glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

        // Bind the Uniform Perspective / Projection Matrix
        glUniformMatrix4fv(UniformProjection, 1, GL_FALSE, glm::value_ptr(Projection));

        //Render the mesh
        Meshes[0]->RenderMesh();

        Model = glm::mat4(1.0f);
        Model = glm::translate(Model, glm::vec3(1.0f, 0.0f, -2.5f));
        Model = glm::rotate(Model, 0.0f, glm::vec3(0.5f, 0.5f, 0.5f));
        Model = glm::scale(Model, glm::vec3(0.25f, 0.25f, 0.25f));
        glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

        Meshes[1]->RenderMesh();

        // Clear the Shader Program
        glUseProgram(0);

        MainWindow.SwapBuffers();
    }

    printf("User closed window.");
    return 0;
}

