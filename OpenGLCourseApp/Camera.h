#pragma once

#include <GL/glew.h>

#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/glm.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera::Camera();
	Camera(glm::vec3 StartPosition, glm::vec3 StartWorldUp, GLfloat StartYaw, GLfloat StartPitch, GLfloat StartMoveSpeed, GLfloat StartTurnSpeed);
	
	void KeyControl(bool* Keys, GLfloat DeltaTime);
	void MouseControl(GLfloat ChangeX, GLfloat ChangeY);

	glm::mat4 CalculateViewMatrix();

	~Camera();

private:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 CameraUp;
	glm::vec3 WorldUp;

	GLfloat Pitch;
	GLfloat Yaw;

	GLfloat MoveSpeed;
	GLfloat TurnSpeed;

	void Update();
};

