#include "GLWindow.h"

GLWindow::GLWindow()
{
	Width = 800;
	Height = 800;
}

GLWindow::GLWindow(GLint WindowWidth, GLint WindowHeight)
{
	Width = WindowWidth;
	Height = WindowHeight;
}

int GLWindow::Initialize()
{// Initialize GLFW
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
    MainWindow = glfwCreateWindow(Width, Height, "Test Window", NULL, NULL);
    if (!MainWindow)
    {
        printf("GLFW Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
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

    //Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    // Create viewport & setup size
    glViewport(0, 0, BufferWidth, BufferHeight);
}

GLWindow::~GLWindow()
{
    glfwDestroyWindow(MainWindow);
    glfwTerminate();
}
