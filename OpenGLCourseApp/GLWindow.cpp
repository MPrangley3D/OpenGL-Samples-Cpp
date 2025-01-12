#include "GLWindow.h"

GLWindow::GLWindow() :
    Width(800),
    Height(600),
    LastX(0.0f),
    LastY(0.0f),
    ChangeX(0.0f),
    ChangeY(0.0f),
    MainWindow(nullptr),
    BufferHeight(0),
    BufferWidth(0)
{

    for (size_t i = 0; i < 1024; i++)
    {
        Keys[i] = 0;
    }
    MouseInitialized = false;
}

GLWindow::GLWindow(GLint WindowWidth, GLint WindowHeight) : 
    Width(WindowWidth),
    Height(WindowHeight),
    LastX(0.0f),
    LastY(0.0f),
    ChangeX(0.0f),
    ChangeY(0.0f),
    MainWindow(nullptr),
    BufferHeight(0),
    BufferWidth(0)
{
    for (size_t i = 0; i < 1024; i++)
    {
        Keys[i] = 0;
    }
    MouseInitialized = false;
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

    // Handle KB + Mouse Inputs
    CreateCallbacks();

    // Lock cursor to window & hide cursor / prevent clicks
    glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // Set user pointer for window, so the static function for key input can access this window
    glfwSetWindowUserPointer(MainWindow, this);
}

GLfloat GLWindow::GetChangeX()
{
    GLfloat TheChange = ChangeX;
    ChangeX = 0.0f;
    return TheChange;
}

GLfloat GLWindow::GetChangeY()
{
    GLfloat TheChange = ChangeY;
    ChangeY = 0.0f;
    return TheChange;
}

void GLWindow::HandleKeys(GLFWwindow* Window, int Key, int Code, int Action, int Mode)
{
    // Gets the user pointer we set above with 'glfwSetWindowUserPointer'
    GLWindow* TheWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(Window));

    if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, GL_TRUE);
    }

    if (Key >= 0 && Key < 1024)
    {
        if (Action == GLFW_PRESS)
        {
            TheWindow->Keys[Key] = true;
            printf("Pressed %d\n", Key);
        }
        else if (Action == GLFW_RELEASE)
        {
            TheWindow->Keys[Key] = false;
            printf("Released %d\n", Key);
        }
    }
}

void GLWindow::HandleMouse(GLFWwindow* Window, double PosX, double PosY)
{
    // Gets the user pointer we set above with 'glfwSetWindowUserPointer'
    GLWindow* TheWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(Window));

    if (!TheWindow->MouseInitialized)
    {
        TheWindow->LastX = PosX;
        TheWindow->LastY = PosY;
        TheWindow->MouseInitialized = true;
    }

    TheWindow->ChangeX = PosX - TheWindow->LastX;
    // Inverted Y
    TheWindow->ChangeY = TheWindow->LastY - PosY;

    TheWindow->LastX = PosX;
    TheWindow->LastY = PosY;

    //printf("X:%.2f, Y:%.2f\n",TheWindow->ChangeX, TheWindow->ChangeY);
}

void GLWindow::CreateCallbacks()
{
    glfwSetKeyCallback(MainWindow, HandleKeys);
    glfwSetCursorPosCallback(MainWindow, HandleMouse);
}

GLWindow::~GLWindow()
{
    glfwDestroyWindow(MainWindow);
    glfwTerminate();
}
