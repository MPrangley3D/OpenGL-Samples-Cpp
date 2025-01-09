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

	bool GetShouldCloseWindow() { return glfwWindowShouldClose(MainWindow); }

	void SwapBuffers() { glfwSwapBuffers(MainWindow); };

	~GLWindow();

private:
	GLFWwindow* MainWindow;
	GLint Width;
	GLint Height;
	GLint BufferWidth;
	GLint BufferHeight;
};

