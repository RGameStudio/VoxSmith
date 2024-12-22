#pragma once

#include <glm/glm.hpp>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct VOX_SMITH_API Plane
	{
		glm::vec3 pos;
		glm::vec3 normal = {0.0f, 0.1f, 0.0f};
	};

	struct VOX_SMITH_API Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane nearFace;
		Plane farFace;
	};

	class VOX_SMITH_API Camera
	{
	public:
		Camera(const glm::vec3 pos, const size_t width, const size_t height);
		~Camera() = default;

		void updateCameraPos(const glm::vec3& dv, const float dt);
		void updateCameraAngle(const float xPos, const float yPos);
		Frustum getFrustum();

		inline glm::mat4 getProjection() const { return m_projection; }
		inline glm::mat4 getView() const { return m_view; }
		inline glm::vec3 getPos() const { return m_pos; }
		inline glm::vec3 getFront() const { return m_front; }
		inline float getViewDistance() const { return m_viewDistance; }

	private:
		glm::mat4 m_projection;
		glm::mat4 m_view;
		glm::vec3 m_pos;
		glm::vec3 m_front;
		glm::vec3 m_right;
		glm::vec3 m_up;

		float m_lastX;
		float m_lastY;

		float m_sensitivity = 0.1f;

		bool m_firstMove = false;
		float m_near = 0.1f;
		float m_far = 100.0f;
		float m_fov = 45.0f;
		float m_yaw = -90.0f;
		float m_pitch = 0.0f;
		float m_aspect;

		float m_speed = 0.0f;
		float m_viewDistance = 0.0f;

		friend class UICanvasIMGUI;
	};
}