#pragma once

#include <glm/glm.hpp>

#include "../Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Camera
	{
	public:
		Camera(const glm::vec3 pos, const size_t width, const size_t height);
		~Camera() = default;

		void updateCameraPos(const glm::vec3& dv, const float dt);
		void updateCameraAngle(const float xPos, const float yPos);

		inline glm::mat4 getProjection() const { return m_projection; }
		inline glm::mat4 getView() const { return m_view; }
		inline glm::vec3 getPos() const { return m_pos; }
		inline glm::vec3 getFront() const { return m_front; }
		inline glm::vec3 getDir() const { return m_dir; }

	private:
		glm::mat4 m_projection;
		glm::mat4 m_view;
		glm::vec3 m_pos;
		glm::vec3 m_front;
		glm::vec3 m_dir;

		float m_lastX;
		float m_lastY;

		float m_sensitivity = 0.1f;

		bool m_firstMove = false;
		float m_yaw = -90.0f;
		float m_pitch = 0.0f;
	};
}