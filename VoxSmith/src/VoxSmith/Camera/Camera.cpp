#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

using namespace VoxSmith;

constexpr float g_fov = 45.0f;

Camera::Camera(const glm::vec3 pos, const size_t width, const size_t height)
	: m_pos(pos)
{
	m_projection = glm::perspective(glm::radians(g_fov), static_cast<float>(width) / height, 0.1f, 100.0f);
	m_view = glm::lookAt(m_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

void Camera::update(float dt)
{

}