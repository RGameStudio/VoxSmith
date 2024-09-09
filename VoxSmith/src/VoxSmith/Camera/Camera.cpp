#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

using namespace VoxSmith;

constexpr float g_fov = 45.0f;

constexpr glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float g_near = 0.1f;
constexpr float g_far = 500.0f;

Camera::Camera(const glm::vec3 pos, const size_t width, const size_t height)
	: m_pos(pos)
	, m_front({ 20.0f, -10.0f, 20.0f })
	, m_dir(glm::normalize(m_front - m_pos))
	, m_lastX(width / 2)
	, m_lastY(height / 2)
	, m_yaw(-90.0f)
	, m_pitch(0.0f)
	, m_firstMove(true)
{
	m_projection = 
		glm::perspective(glm::radians(g_fov), static_cast<float>(width) / static_cast<float>(height), g_near, g_far);
	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::updateCameraPos(const glm::vec3& dv, const float dt)
{
	m_pos += dv * dt;
	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::updateCameraAngle(const float xPos, const float yPos)
{
	if (m_firstMove)
	{
		m_lastX = xPos;
		m_lastY = yPos;
		m_firstMove = false;
	}

	const float xOffset = m_sensitivity * (xPos - m_lastX);
	const float yOffset = m_sensitivity * (m_lastY - yPos);

	m_lastX = xPos;
	m_lastY = yPos;
	
	m_yaw += xOffset;
	m_pitch += yOffset;

	if (m_pitch > 89.0f)
	{
		m_pitch = 89.0f;
	}
	if (m_pitch < -89.0f)
	{
		m_pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	direction.y = sin(glm::radians(m_pitch));
	direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(direction);
	m_dir = glm::normalize(m_front);
}
