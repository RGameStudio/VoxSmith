#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

using namespace VoxSmith;

constexpr float g_fov = 45.0f;

constexpr glm::vec3 g_up = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float g_near = 0.1f;
constexpr float g_far = 400.0f;

Camera::Camera(const glm::vec3 pos, const size_t width, const size_t height)
	: m_pos(pos)
	, m_front({ 20.0f, -10.0f, 20.0f })
	, m_up(g_up)
	, m_right(glm::cross(m_front, m_up))
	, m_lastX(width / 2)
	, m_lastY(height / 2)
	, m_near(g_near)
	, m_far(g_far)
	, m_aspect(static_cast<float>(width) / static_cast<float>(height))
	, m_yaw(-90.0f)
	, m_pitch(0.0f)
	, m_firstMove(true)
	, m_viewDistance(g_far - g_near)
{
	m_projection =
		glm::perspective(glm::radians(g_fov), m_aspect, m_near, m_far);
	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::updateCameraPos(const glm::vec3& dv, const float dt)
{
	m_pos += m_speed * dv * dt;
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

	m_front = {
		cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
		sin(glm::radians(m_pitch)),
		sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))
	};
	m_front = glm::normalize(m_front);
	m_right = glm::normalize(glm::cross(m_front, g_up));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}

Frustum Camera::getFrustum()
{
	Frustum frustum;
	const float halfHeight = m_far * glm::tan(glm::radians(m_fov) * 0.5f);
	const float halfWidth = halfHeight * m_aspect;
	const glm::vec3 frontFar = m_far * m_front;

	frustum.nearFace = { m_pos + m_near * m_front, glm::normalize(m_front) };
	frustum.farFace = { m_pos + m_far * m_front, -frustum.nearFace.normal };

	frustum.rightFace = { m_pos, glm::normalize(glm::cross(frontFar - m_right * halfWidth, m_up)) };
	frustum.leftFace = { m_pos, glm::normalize(glm::cross(m_up, frontFar + m_right * halfWidth)) };

	frustum.topFace = { m_pos, glm::normalize(glm::cross(m_right, frontFar - m_up * halfHeight)) };
	frustum.bottomFace = { m_pos, glm::normalize(glm::cross(frontFar + m_up * halfHeight, m_right)) };

	return frustum;
}

bool Plane::pointOnFacePlane(const glm::vec3& pos_) const
{
	const float r = 16 * glm::abs(normal.x) + 16 * glm::abs(normal.y) + 16 * glm::abs(normal.z);
	return glm::dot(normal, pos_) - glm::dot(pos, normal) + r >= 0;
}
