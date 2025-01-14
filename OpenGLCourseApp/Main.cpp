#define STB_IMAGE_IMPLEMENTATION

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
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"

GLWindow MainWindow;
std::vector<Mesh*> Meshes;
std::vector<Shader> Shaders;
Camera MyCamera;
Light AmbientLight;

Texture BrickTexture;
Texture DirtTexture;

Material ShinyMaterial;
Material DullMaterial;

GLfloat DeltaTime = 0.0f;
GLfloat LastTime = 0.0f;

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

void CalculateAverageNormals(unsigned int* Indicies, unsigned int IndicieCount, 
                                GLfloat* Verticies, unsigned int VerticieCount, 
                                unsigned int VertexLength, unsigned int NormalOffset)
{
    for (size_t i = 0; i < IndicieCount; i += 3)
    {
        // Accesses the float data of the vertex for the given indicie
        unsigned int in0 = Indicies[i] * VertexLength;
        unsigned int in1 = Indicies[i + 1] * VertexLength;
        unsigned int in2 = Indicies[i + 2] * VertexLength;

        // Calculate the unitized Normal
        glm::vec3 v1(Verticies[in1] - Verticies[in0], Verticies[in1 + 1] - Verticies[in0 + 1], Verticies[in1 + 2] - Verticies[in0 + 2]);
        glm::vec3 v2(Verticies[in2] - Verticies[in0], Verticies[in2 + 1] - Verticies[in0 + 1], Verticies[in2 + 2] - Verticies[in0 + 2]);
        glm::vec3 Normal = glm::cross(v1, v2);
        Normal = glm::normalize(Normal);

        // Shift index registers over to their respective normal vertex indicies  (NX, NY, NZ in GeometryVerticies below) 
        in0 += NormalOffset;
        in1 += NormalOffset;
        in2 += NormalOffset;

        // Store calculated normals in the GeometryVerticies data set below
        Verticies[in0] += Normal.x; 
        Verticies[in0 + 1] += Normal.y; 
        Verticies[in0 + 2] += Normal.z;
        Verticies[in1] += Normal.x; 
        Verticies[in1 + 1] += Normal.y; 
        Verticies[in1 + 2] += Normal.z;
        Verticies[in2] += Normal.x; 
        Verticies[in2 + 1] += Normal.y; 
        Verticies[in2 + 2] += Normal.z;
    }

    // Iterate over and normalize the normal vectors
    for (size_t i = 0; i < VerticieCount / VertexLength; i++)
    {
        unsigned int OffsetN = i * VertexLength + NormalOffset;
        glm::vec3 vec(Verticies[OffsetN], Verticies[OffsetN + 1], Verticies[OffsetN + 2]);
        vec = glm::normalize(vec);
        Verticies[OffsetN] = vec.x;
        Verticies[OffsetN + 1] = vec.y;
        Verticies[OffsetN + 2] = vec.z;
    }
}

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
    {//   X      Y     Z       U      V         NX    NY    NZ
         -1.0f, -1.0f, -0.6f,   0.0f, 0.0f,      0.0f, 0.0f, 0.0f,       //0
          0.0f, -1.0f,  1.0f,   0.5f, 0.0f,      0.0f, 0.0f, 0.0f,       //1
          1.0f, -1.0f, -0.6f,   1.0f, 0.0f,      0.0f, 0.0f, 0.0f,       //2
          0.0f,  1.0f,  0.0f,   0.5f, 1.0f,      0.0f, 0.0f, 0.0f        //3
    };

    CalculateAverageNormals(Indicies, 12, GeometryVerticies, 32, 8, 5);

    Mesh* Object1 = new Mesh();
    Object1->CreateMesh(GeometryVerticies, Indicies, 32, 12);
    Meshes.push_back(Object1);

    Mesh* Object2 = new Mesh();
    Object2->CreateMesh(GeometryVerticies, Indicies, 32, 12);
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
    MainWindow = GLWindow(1366, 768);
    MainWindow.Initialize();

    CreateObjects();
    CreateShaders();
    MyCamera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.0f, 0.1f);

    BrickTexture = Texture("Textures/brick.png");
    BrickTexture.LoadTexture();
    DirtTexture = Texture("Textures/dirt.png");
    DirtTexture.LoadTexture();

    ShinyMaterial = Material(1.0f, 16);
    DullMaterial = Material(0.3f, 4);

    AmbientLight = Light(0.9f,  0.9f,  1.0f, 0.1f, 
                         2.0f, -1.0f, -2.0f, 0.7f);

    // Default values for Uniform IDs, updates in While loop per-shader.
    GLuint UniformProjection = 0;
    GLuint UniformView = 0;
    GLuint UniformModel = 0;
    GLuint UniformEyePosition = 0;
    GLuint UniformAmbientColor = 0;
    GLuint UniformAmbientIntensity = 0;
    GLuint UniformDiffuseIntensity = 0;
    GLuint UniformLightDirection = 0;
    GLuint UniformSpecularIntensity = 0;
    GLuint UniformShininess = 0;

    // We only need to set up Projection once, so we do it here rather than in the While loop
    glm::mat4 Projection = glm::perspective(45.0f, MainWindow.GetBufferWidth() / MainWindow.GetBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while (!MainWindow.GetShouldCloseWindow())
    {
        // Get frame time in seconds
        GLfloat Now = glfwGetTime(); // SDL_GetPerformanceCounter() for SDL (Must be converted to seconds for SDL)
        DeltaTime = Now - LastTime;  // (Now - LastTime) * 1000 / SDL_GetPerformanceFrequency();
        LastTime = Now;

        // Get + Handle User Input Events
        glfwPollEvents();

        // Pass key inputs from Window to the Camera
        MyCamera.KeyControl(MainWindow.GetKeys(), DeltaTime);
        MyCamera.MouseControl(MainWindow.GetChangeX(), MainWindow.GetChangeY());

        // Clear window
        glClearColor(0.0f,0.0f,0.0f,1.0f);

        // Clear the color & depth buffer bits
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Assign the Shader Program
        Shaders[0].UseShader();

        // Set uniform IDs based on shader IDs
        UniformModel = Shaders[0].GetModelLocation();
        UniformProjection = Shaders[0].GetProjectionLocation();
        UniformView = Shaders[0].GetViewLocation();
        UniformEyePosition = Shaders[0].GetEyePositionLocation();
        UniformAmbientColor = Shaders[0].GetAmbientColorLocation();
        UniformAmbientIntensity = Shaders[0].GetAmbientIntensityLocation();
        UniformDiffuseIntensity = Shaders[0].GetDiffuseIntensityLocation();
        UniformLightDirection = Shaders[0].GetLightDirectionLocation();
        UniformSpecularIntensity = Shaders[0].GetSpecularIntensityLocation();
        UniformShininess = Shaders[0].GetShininessLocation();

        // Sets up light in shaders
        AmbientLight.UseLight(UniformAmbientIntensity, UniformAmbientColor, UniformDiffuseIntensity, UniformLightDirection);

        // Bind the Uniform Perspective / Projection Matrix
        glUniformMatrix4fv(UniformProjection, 1, GL_FALSE, glm::value_ptr(Projection));

        // Bind the Camera / View Matrix
        glUniformMatrix4fv(UniformView, 1, GL_FALSE, glm::value_ptr(MyCamera.CalculateViewMatrix()));

        // Bind the Eye Position based on the Camera location
        glUniform3f(UniformEyePosition, MyCamera.GetCameraPosition().x, MyCamera.GetCameraPosition().y, MyCamera.GetCameraPosition().z);

        // Defines a 4x4 matrix for the Model Matrix (1.0f) initializes as a Identity Matrix
        glm::mat4 Model(1.0f);

        // Update the Transform by the Model Matrix
        // Note these are applied in reserve order to the object
        Model = glm::translate(Model, glm::vec3(-2.0f, 0.0f, -2.5f));
        // Model = glm::rotate(Model, 0.0f, glm::vec3(0.5f, 0.5f, 0.5f)); 
        // Model = glm::scale(Model, glm::vec3(0.25f, 0.25f, 0.25f));
        
        // Bind the Uniform Model Matrix
        glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

        // Use Brick Texture
        BrickTexture.UseTexture();

        // Use shiny Material
        ShinyMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

        // Render mesh 0
        Meshes[0]->RenderMesh();

        // Refresh & create new Model info for 2nd Mesh, View & Projection is re-used
        Model = glm::mat4(1.0f);
        Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, -2.5f));
        // Model = glm::rotate(Model, 0.0f, glm::vec3(0.5f, 0.5f, 0.5f));
        // Model = glm::rotate(Model, 0.0f, glm::vec3(0.5f, 0.5f, 0.5f));
        // Model = glm::scale(Model, glm::vec3(0.25f, 0.25f, 0.25f));
        glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

        // Use Dirt Texture
        DirtTexture.UseTexture();

        // Use dull Material
        DullMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

        // Render mesh 1
        Meshes[1]->RenderMesh();

        // Clear the Shader Program
        glUseProgram(0);

        MainWindow.SwapBuffers();
    }

    printf("User closed window.");
    return 0;
}

