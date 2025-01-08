#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;


int main()
{
    //Initialize GLFW
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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    return 0;
}

