#pragma once
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
	GLWindow();
	GLWindow(GLint WindowWidth, GLint WindowHeight);

	int Initialize();

	GLfloat GetBufferWidth() { return BufferWidth; }
	GLfloat GetBufferHeight() { return BufferHeight; }

	bool* GetKeys() { return Keys; }
	GLfloat GetChangeX();
	GLfloat GetChangeY();

	bool GetShouldCloseWindow() { return glfwWindowShouldClose(MainWindow); }

	void SwapBuffers() { glfwSwapBuffers(MainWindow); };

	~GLWindow();

private:
	GLFWwindow* MainWindow;
	GLint Width;
	GLint Height;
	GLint BufferWidth;
	GLint BufferHeight;

	// Key Presses
	bool Keys[1024];

	// Mouse Positions
	GLfloat LastX;
	GLfloat LastY;
	GLfloat ChangeX;
	GLfloat ChangeY;
	bool MouseInitialized;

	// Static allows calls with GLWindow::HandleKeys without a reference to this window object
	static void HandleKeys(GLFWwindow* Window, int Key, int Code, int Action, int Mode);
	static void HandleMouse(GLFWwindow* Window, double PosX, double PosY);
	void CreateCallbacks();

};

