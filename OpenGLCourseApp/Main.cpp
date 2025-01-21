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

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Skybox.h"
#include "Model.h"

#include "assimp/Importer.hpp"

const float ToRadians = 3.14159265f / 180.0f;

GLWindow MainWindow;
std::vector<Mesh*> Meshes;
std::vector<Shader> Shaders;
Shader DirectionalShadowShader;
Shader OmniShadowShader;

Camera MyCamera;

DirectionalLight MainLight;
PointLight PointLights[MAX_POINT_LIGHTS];
SpotLight SpotLights[MAX_SPOT_LIGHTS];

Texture BrickTexture;
Texture DirtTexture;
Texture PlainTexture;
Texture SoilTexture;

Material ShinyMaterial;
Material DullMaterial;

Skybox MySkybox;

Model XWing;
Model Chopper;

GLfloat DeltaTime = 0.0f;
GLfloat LastTime = 0.0f;

GLfloat ChopperAngle = 0.0f;

// Light Settings
unsigned int PointLightCount = 3;
unsigned int SpotLightCount = 3;

// Default values for Uniform IDs, updates in While loop per-shader.
GLuint UniformProjection = 0;
GLuint UniformView = 0;
GLuint UniformModel = 0;
GLuint UniformEyePosition = 0;
GLuint UniformSpecularIntensity = 0;
GLuint UniformShininess = 0;
GLuint UniformOmniLightPosition = 0;
GLuint UniformFarPlane = 0;

// Shader code file paths
static const char* VertexShader = "Shaders/shader.vert";
static const char* FragmentShader = "Shaders/shader.frag";
static const char* DirectionalVertexShader = "Shaders/directional_shadow_map.vert";
static const char* DirectionalFragmentShader = "Shaders/directional_shadow_map.frag";
static const char* OmniVertexShader = "Shaders/omni_shadow_map.vert";
static const char* OmniFragmentShader = "Shaders/omni_shadow_map.frag";
static const char* OmniGeometryShader = "Shaders/omni_shadow_map.geom";

int ViewportWidth = 1366;
int ViewportHeight = 768;

bool bEnableFlashlight = false;

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

        // Shift index registers over to their respective normal vertex indicies  (NX, NY, NZ in GeometryVertices below) 
        in0 += NormalOffset;
        in1 += NormalOffset;
        in2 += NormalOffset;

        // Store calculated normals in the GeometryVertices data set below
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

    GLfloat GeometryVertices[] = 
    {//   X      Y     Z       U      V         NX    NY    NZ
         -1.0f, -1.0f, -0.6f,   0.0f, 0.0f,      0.0f, 0.0f, 0.0f,       //0
          0.0f, -1.0f,  1.0f,   0.5f, 0.0f,      0.0f, 0.0f, 0.0f,       //1
          1.0f, -1.0f, -0.6f,   1.0f, 0.0f,      0.0f, 0.0f, 0.0f,       //2
          0.0f,  1.0f,  0.0f,   0.5f, 1.0f,      0.0f, 0.0f, 0.0f        //3
    };


    //Specify the point sets that make our Floor
    unsigned int FloorIndicies[] =
    {
        0,2,1,
        1,2,3
    };

    GLfloat FloorVertices[] =
    {
        -100.0f, 0.0f, -100.0f,   0.0f, 0.0f,     0.0f, -1.0f, 0.0f,  //0
         100.0f, 0.0f, -100.0f,  10.0f, 0.0f,     0.0f, -1.0f, 0.0f,  //1
        -100.0f, 0.0f,  100.0f,   0.0f, 10.0f,    0.0f, -1.0f, 0.0f,  //2
         100.0f, 0.0f,  100.0f,  10.0f, 10.0f,    0.0f, -1.0f, 0.0f   //3
    };

    CalculateAverageNormals(Indicies, 12, GeometryVertices, 32, 8, 5);

    Mesh* Object1 = new Mesh();
    Object1->CreateMesh(GeometryVertices, Indicies, 32, 12);
    Meshes.push_back(Object1);

    Mesh* Object2 = new Mesh();
    Object2->CreateMesh(GeometryVertices, Indicies, 32, 12);
    Meshes.push_back(Object2);

    Mesh* Object3 = new Mesh();
    Object3->CreateMesh(FloorVertices, FloorIndicies, 32, 6);
    Meshes.push_back(Object3);
}

void CreateShaders()
{
    // Base Shader for Phong shading
    Shader* Shader1 = new Shader();
    Shader1->CreateFromFiles(VertexShader, FragmentShader);
    Shaders.push_back(*Shader1);

    // Shader for the Directional Shadow Map
    DirectionalShadowShader = Shader();
    DirectionalShadowShader.CreateFromFiles(DirectionalVertexShader, DirectionalFragmentShader);

    // Shader for the Omnidirectional Shadows CubeMap
    OmniShadowShader = Shader();
    OmniShadowShader.CreateFromFiles(OmniVertexShader, OmniFragmentShader, OmniGeometryShader);
}

void RenderScene()
{
    //----------------[Start Pyramid 1]--------------------------------
    // Defines a 4x4 matrix for the Model Matrix (1.0f) initializes as a Identity Matrix
    glm::mat4 Model(1.0f);

    // Update the Transform by the Model Matrix
    // Note these are applied in reverse order to the object
    Model = glm::translate(Model, glm::vec3(-2.0f, 0.0f, -2.5f));

    // Bind the Uniform Model Matrix
    glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

    // Use Brick Texture
    BrickTexture.UseTexture();

    // Use shiny Material
    DullMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

    // Render mesh 0
    Meshes[0]->RenderMesh();
    //----------------[End Pyramid 1]--------------------------------

    //----------------[Start Pyramid 2]--------------------------------
    // Refresh & create new Model info for 2nd Mesh, View & Projection is re-used
    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, -2.5f));
    glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

    // Use Dirt Texture
    DirtTexture.UseTexture();

    // Use dull Material
    DullMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

    // Render mesh
    Meshes[1]->RenderMesh();
    //----------------[End Pyramid 2]--------------------------------

    //----------------[Start Ground]--------------------------------
    // Refresh & create new Model info for floor Mesh, View & Projection is re-used
    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

    // Use Dirt Texture
    SoilTexture.UseTexture();

    // Use dull Material
    ShinyMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

    // Render mesh
    Meshes[2]->RenderMesh();

    //----------------[End Ground]--------------------------------

    //----------------[Start X-Wing]--------------------------------
    // Refresh & create new Model info for floor Mesh, View & Projection is re-used
    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, glm::vec3(-7.0f, 0.0f, 5.0f));
    Model = glm::scale(Model, glm::vec3(0.006f, 0.006f, 0.006f));
    glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

    // Use shiny Material
    ShinyMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

    // Render mesh
    XWing.RenderModel();
    //----------------[End X-Wing]--------------------------------


    //----------------[Start Chopper]--------------------------------
    // Refresh & create new Model info for floor Mesh, View & Projection is re-used
    ChopperAngle += 0.1f;
    if (ChopperAngle > 360)
    {
        ChopperAngle = 0.1;
    }

    Model = glm::mat4(1.0f);
    Model = glm::rotate(Model, ChopperAngle * ToRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(-8.0f, 2.0f, 0.0f));
    Model = glm::rotate(Model, 270 * ToRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, 180 * ToRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::rotate(Model, -30 * ToRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::scale(Model, glm::vec3(0.2f, 0.2f, 0.2f));
    glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));

    // Use shiny Material
    DullMaterial.UseMaterial(UniformSpecularIntensity, UniformShininess);

    // Render mesh
    Chopper.RenderModel();
    //----------------[End Chopper]--------------------------------
}

void DirectionalShadowMapPass(DirectionalLight* Light)
{
    DirectionalShadowShader.UseShader();

    // Sets the viewport to the same dimensions as the framebuffer
    glViewport(0, 0, Light->GetShadowMap()->GetShadowWidth(), Light->GetShadowMap()->GetShadowHeight());

    // Enable depth buffer writing to shadow map
    Light->GetShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    // Set up uniforms for shader
    UniformModel = DirectionalShadowShader.GetModelLocation();
    DirectionalShadowShader.SetDirectionalLightTransform(&Light->CalculateLightTransform());

    // Validate the Shader before Rendering
    DirectionalShadowShader.ValidateShader();

    // Render the depth pass
    RenderScene();

    // Unbinds frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* Light)
{
    OmniShadowShader.UseShader();

    // Sets the viewport to the same dimensions as the framebuffer
    glViewport(0, 0, Light->GetShadowMap()->GetShadowWidth(), Light->GetShadowMap()->GetShadowHeight());

    // Enable depth buffer writing to shadow map
    Light->GetShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    // Set up uniforms for shader
    UniformModel = OmniShadowShader.GetModelLocation();
    UniformOmniLightPosition = OmniShadowShader.GetOmniLightPositionLocation();
    UniformFarPlane = OmniShadowShader.GetFarPlaneLocation();
    glUniform3f(UniformOmniLightPosition, Light->GetPosition().x, Light->GetPosition().y, Light->GetPosition().z);
    glUniform1f(UniformFarPlane, Light->GetFarPlane());
    OmniShadowShader.SetOmniLightMatrices(Light->CalculateLightTransforms());

    // Validate the Shader before Rendering
    OmniShadowShader.ValidateShader();

    // Render the depth pass
    RenderScene();

    // Unbinds frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix)
{
    // Verify viewport settings (In case they were changed by depth buffer/etc
    glViewport(0, 0, ViewportWidth, ViewportHeight);

    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Clear the color & depth buffer bits
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Skybox
    printf("Drawing Skybox...\n");
    MySkybox.DrawSkybox(ViewMatrix, ProjectionMatrix);

    // Assign the Shader Program
    Shaders[0].UseShader();

    // Set uniform IDs based on shader IDs
    UniformModel = Shaders[0].GetModelLocation();
    UniformProjection = Shaders[0].GetProjectionLocation();
    UniformView = Shaders[0].GetViewLocation();
    UniformEyePosition = Shaders[0].GetEyePositionLocation();
    UniformSpecularIntensity = Shaders[0].GetSpecularIntensityLocation();
    UniformShininess = Shaders[0].GetShininessLocation();

    // Sets up light in shaders
    Shaders[0].SetDirectionalLight(&MainLight);
    Shaders[0].SetPointLights(PointLights, PointLightCount, 3, 0);
    Shaders[0].SetSpotLights(SpotLights, SpotLightCount, 3 + PointLightCount, PointLightCount);
    Shaders[0].SetDirectionalLightTransform(&MainLight.CalculateLightTransform());

    MainLight.GetShadowMap()->Read(GL_TEXTURE2);

    // Set GL_TEXTURE1 as Texture and GL_TEXTURE2 as the Shadow Map (0 reserved for defaults)
    Shaders[0].SetTexture(1);
    Shaders[0].SetDirectionalShadowMap(2);

    // Bind the Uniform Perspective / Projection Matrix
    glUniformMatrix4fv(UniformProjection, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

    // Bind the Camera / View Matrix
    glUniformMatrix4fv(UniformView, 1, GL_FALSE, glm::value_ptr(ViewMatrix));

    // Bind the Eye Position based on the Camera location
    glUniform3f(UniformEyePosition, MyCamera.GetCameraPosition().x, MyCamera.GetCameraPosition().y, MyCamera.GetCameraPosition().z);

    // Attach Flashlight Spotlight
    if (bEnableFlashlight)
    {
        glm::vec3 FlashlightOffset = MyCamera.GetCameraPosition();
        FlashlightOffset.y -= 0.1f;
        SpotLights[1].SetFlash(FlashlightOffset, MyCamera.GetCameraDirection());
        SpotLights[1].ToggleSpotlight(bEnableFlashlight);
    }
    else
    {
        SpotLights[1].ToggleSpotlight(bEnableFlashlight);
    }

    
    // Validate the Shader before Rendering
    Shaders[0].ValidateShader();

    // Render the depth pass
    RenderScene();
}

int main()
{
    MainWindow = GLWindow(ViewportWidth, ViewportHeight);
    MainWindow.Initialize();

    CreateObjects();
    CreateShaders();
    MyCamera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.0f, 0.1f);

    BrickTexture = Texture("Textures/brick.png");
    BrickTexture.LoadAlphaTexture();
    DirtTexture = Texture("Textures/dirt.png");
    DirtTexture.LoadAlphaTexture();
    PlainTexture = Texture("Textures/plain.png");
    PlainTexture.LoadAlphaTexture();
    SoilTexture = Texture("Textures/soil.jpg");
    SoilTexture.LoadTexture();

    ShinyMaterial = Material(1.0f, 16);
    DullMaterial = Material(0.3f, 4);

    XWing = Model();
    XWing.LoadModel("Models/x-wing.obj");
    Chopper = Model();
    Chopper.LoadModel("Models/uh60.obj");

    // Params 1-3: Ambient RGB (Line 1)
    // Param 4: Ambient Intensity (Line 2)
    // Param 5: Diffuse Intensity (Line 2)
    // Params 6-8: Direction (Line 3)
    MainLight = DirectionalLight(2048, 2048,
                                 1.0f,  0.55f,  0.3f, 
                                 0.01f, 0.9f,
                                -10.0f,-12.0f, 18.5f);

    // Params 1-2: Shadow Width, Shadow Height (Line 1)
    // Params 3-4: NearClip, FarClip (Line 2)
    // Params 5-7: Ambient RGB (Line 3)
    // Param 8: Ambient Intensity (Line 4)
    // Param 9: Diffuse Intensity (Line 4)
    // Params 10-13: Position (Line 5)
    // Params 14-17: Direction (Line 6)
    // Params 12-15: Constant, Linear, Exponent (Line 7)
    PointLights[0] = PointLight(1024, 1024,
                                0.1f, 100.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f,
                               -5.0f, 3.0f, 0.0f,
                                0.5f, 0.2f, 0.1f);
    
    PointLights[1] = PointLight(1024, 1024,
                                0.01f, 100.0f, 
                                0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f,
                                1.0f, 2.0f, 0.0f,
                                0.3f, 0.1f, 0.1f);

    PointLights[2] = PointLight(1024, 1024,
                                0.1f, 100.0f, 
                                0.0f, 0.0f, 1.0f,
                                0.0f, 0.0f,
                                0.0f, 3.0f, -6.0f,
                                0.5f, 0.2f, 0.1f);

    // Params 1-2: Shadow Width, Shadow Height (Line 1)
    // Params 3-4: NearClip, FarClip (Line 2)
    // Params 5-7: Ambient RGB (Line 3)
    // Param 8: Ambient Intensity (Line 4)
    // Param 9: Diffuse Intensity (Line 4)
    // Params 10-13: Position (Line 5)
    // Params 14-17: Direction (Line 6)
    // Params 12-15: Constant, Linear, Exponent (Line 7)
    // Param 16:  Edge (Line 8)
    SpotLights[0] = SpotLight(1024, 1024,
                                0.1f, 100.0f, 
                                0.0f, 1.0f, 1.0f,
                                0.0f,  0.0f,
                                0.0f,  0.0f,  0.0f,
                                0.0f, -1.0f, -0.5f,
                                0.3f,  0.2f,  0.1f,
                                55.0f);
    // The flashlight Spotlight
    SpotLights[1] = SpotLight(1024, 1024,
                                0.1f, 100.0f, 
                                1.0f, 1.0f, 1.0f,
                                0.0f, 1.0f,
                                5.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f);

    SpotLights[2] = SpotLight(1024, 1024,
                                0.1f, 100.0f, 
                                1.0f, 0.0f, 1.0f,
                                0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f,
                                10.0f, -1.0f, -10.0f,
                                0.3f, 0.2f, 0.1f,
                                15.0f);

    // Setup Skybox Faces
    std::vector<std::string> SkyboxFaces;
 
    // Face: Positive X (Right)
    SkyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    // Face: Negative X (Left)
    SkyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");

    // Face: Positive Y (Up)
    SkyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
    // Face: Negative Y (Down)
    SkyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");

    // Face: Positive Z (Back)
    SkyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    // Face: Negative Z (Front)
    SkyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

    // Construct Skybox
    MySkybox = Skybox(SkyboxFaces);



    // We only need to set up Projection once, so we do it here rather than in the While loop
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), MainWindow.GetBufferWidth() / MainWindow.GetBufferHeight(), 0.1f, 100.0f);

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

        // Toggles Flashlight Spotlight on & off
        if (MainWindow.GetKeys()[GLFW_KEY_F])
        {
            bEnableFlashlight = !bEnableFlashlight;
            MainWindow.GetKeys()[GLFW_KEY_F] = false;
        }

        // Render Passes
        // Directional Shadow Pass
        DirectionalShadowMapPass(&MainLight);
        // Omnidirectional Cube Map Pass - Point Lights
        for (size_t i = 0; i < PointLightCount; i++)
        {
            OmniShadowMapPass(&PointLights[i]);
        }
        // Omnidirectional Cube Map Pass - Spot Lights
        for (size_t i = 0; i < SpotLightCount; i++)
        {
            OmniShadowMapPass(&SpotLights[i]);
        }
        // Phone Shader Render Pass
        RenderPass(Projection, MyCamera.CalculateViewMatrix());
        
        // Clear the Shader Program
        glUseProgram(0);

        MainWindow.SwapBuffers();
    }

    printf("User closed window.");
    return 0;
}

