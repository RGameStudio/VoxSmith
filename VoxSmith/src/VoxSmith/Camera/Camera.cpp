#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

using namespace VoxSmith;

constexpr float g_fov = 45.0f;

constexpr glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float g_near = 0.1f;
constexpr float g_far = 100.0f;

Camera::Camera(const glm::vec3 pos, const size_t width, const size_t height)
	: m_pos(pos)
	, m_front({0.0f, 0.0f, -1.0f})
	, m_dir(glm::normalize((m_front - m_pos)))
{
	m_projection = 
		glm::perspective(glm::radians(g_fov), static_cast<float>(width) / static_cast<float>(height), g_near, g_far);
	m_view = glm::lookAt(m_pos, m_front, m_up);
}

void Camera::update(const glm::vec3& dv, const float dt)
{
	m_pos += dv * dt;
	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
}