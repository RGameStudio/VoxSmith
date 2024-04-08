#pragma once

#include <glm/glm.hpp>

#include "../Core.hpp"

namespace VoxSmith
{
	class Mesh;

	class VOX_SMITH_API Chunk
	{
	public:
		Chunk(const glm::ivec3& size);
		~Chunk() = default;

		void draw();

	private:
		glm::ivec3 m_size;
		glm::vec3 m_pos;

		//Mesh m_mesh;
	};
}