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

		void update(float dt);


	private:
		glm::mat4 m_projection;
		glm::mat4 m_view;
		glm::vec3 m_pos;
	};
}