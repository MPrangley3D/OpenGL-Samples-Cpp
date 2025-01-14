#include "Camera.h"

Camera::Camera(glm::vec3 StartPosition, glm::vec3 StartWorldUp, GLfloat StartYaw, GLfloat StartPitch, GLfloat StartMoveSpeed, GLfloat StartTurnSpeed) :
	Position(StartPosition),
	Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	WorldUp(StartWorldUp),
	Pitch(StartPitch),
	Yaw(StartYaw),
	MoveSpeed(StartMoveSpeed),
	TurnSpeed(StartTurnSpeed)
{
	Update();
}

Camera::Camera()
{
	Update();
}

void Camera::KeyControl(bool* Keys, GLfloat DeltaTime)
{
	GLfloat Velocity = MoveSpeed * DeltaTime;

	// Forward
	if (Keys[GLFW_KEY_W])
	{
		Position += Front * Velocity;
	}
	// Left
	if (Keys[GLFW_KEY_A])
	{
		Position -= Right * Velocity;
	}
	// Back
	if (Keys[GLFW_KEY_S])
	{
		Position -= Front * Velocity;
	}
	// Right
	if (Keys[GLFW_KEY_D])
	{
		Position += Right * Velocity;
	}

	// Down
	if (Keys[GLFW_KEY_Q])
	{
		Position -= WorldUp * Velocity;
	}

	// Up
	if (Keys[GLFW_KEY_E])
	{
		Position += WorldUp * Velocity;
	}
}

void Camera::MouseControl(GLfloat ChangeX, GLfloat ChangeY)
{
	ChangeX *= TurnSpeed;
	ChangeY *= TurnSpeed;

	Yaw += ChangeX;
	Pitch += ChangeY;

	// Clamp Pitch within ±89°
	if (Pitch > 89.0f)
	{
		Pitch = 89.0f;
	}
	if (Pitch < -89.0f)
	{
		Pitch = -89.0f;
	}

	Update();
}

glm::mat4 Camera::CalculateViewMatrix()
{
	return glm::lookAt(Position, Position + Front, CameraUp);
}

glm::vec3 Camera::GetCameraPosition()
{
	return Position;
}

void Camera::Update()
{
	// Update Camera Vectors
	Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front.y = sin(glm::radians(Pitch));
	Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(Front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	CameraUp = glm::normalize(glm::cross(Right, Front));
}
