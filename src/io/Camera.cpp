#include "Camera.h"




Camera::Camera(glm::vec3 position)
	: cameraPos(position), 
	worldUp(glm::vec3(0.0f,1.0f,0.0f)),
	yaw (90.0f),
	pitch(0.0f),
	speed(2.5f),
	zoom(45.0f),
	cameraFront(glm::vec3(1.0f,0.0f,0.0f))
{
	updateCameraVectors();
}

Camera::Camera(glm::vec3 position, float vertical, float horizont) 
	: cameraPos(position),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(vertical),
	pitch(horizont),
	speed(2.5f),
	zoom(45.0f),
	cameraFront(glm::vec3(1.0f, 0.0f, 0.0f)){
	updateCameraVectors();
}

void Camera::updateCameraDirection(double dx, double dy)  // ����� �� ������� ������ 
{
	yaw += dx;
	pitch += dy;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch< -89.0f)
	{
		pitch = -89.0f;
	}

	updateCameraVectors();

}

void Camera::updateCameraPos(CameraDirection direction, double dt) //����� �� ������� ������ 
{
	float velocity = (float)dt * speed;

	switch (direction)
	{
	case CameraDirection::FORWARD :
		cameraPos += cameraFront * velocity;
		break;

	case CameraDirection::BACKWARD:
		cameraPos -= cameraFront * velocity;
		break;

	case CameraDirection::RIGHT:
		cameraPos += cameraRight * velocity;
		break;
	case CameraDirection::LEFT:
		cameraPos -= cameraRight * velocity;
		break;
	case CameraDirection::UP:
		cameraPos += worldUp * velocity;
		break;
	case CameraDirection::DOWN:
		cameraPos -= worldUp * velocity;
		break;
	}

}

void Camera::updateCameraZoom(double dy)
{
	if (zoom >= 1.0f && zoom <= 45.0f) {
		zoom -= dy;

	}
	else if (zoom <1.0f) 
	{
		zoom = 1.0f;
	}
	else{ // >45.0f
		zoom = 45.0f;
	}
}

float Camera::getZoom()
{
	return zoom;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	//forward = normalize(cameraPos - target); // ��� -cameraFront
	//right = normalize(cross(cameraUp, forward));
	//up = cross(forward, right);
	/*glm::mat4 ViewMatrix = {
	right.x,   up.x,   -forward.x,  0.0f,
	right.y,   up.y,   -forward.y,  0.0f,
	right.z,   up.z,   -forward.z,  0.0f,
   -dot(right, cameraPos), -dot(up, cameraPos), dot(forward, cameraPos), 1.0f
	};*/
}

void Camera::updateCameraVectors()
{
	glm::vec3 direction;
	// ��������� ����������� ������� �� ���� ����, ��������� ���� yaw (�������������� �������) � pitch (������������ ������).
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));  // �������������� ������������
	direction.y = sin(glm::radians(pitch));                          // ������������ ������������
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));  // ������� (������-�����)


	cameraFront = glm::normalize(direction);


	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));  //������� ���� ������� ���� ��� ��������� ������ ����������� �������
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront)); 
}
