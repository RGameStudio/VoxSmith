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

		void update(const glm::vec3& dv, const float dt);

		inline glm::mat4 getProjection() const { return m_projection; }
		inline glm::mat4 getView() const { return m_view; }
		inline glm::vec3 getPos() const { return m_pos; }

	private:
		glm::mat4 m_projection;
		glm::mat4 m_view;
		glm::vec3 m_pos;
		glm::vec3 m_front;
	};
}