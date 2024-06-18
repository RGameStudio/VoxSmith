#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Buffer.hpp"

#include "../Chunk/Voxel.hpp"
#include "../Core.hpp"

namespace VoxSmith
{
	class Renderer;

	struct VOX_SMITH_API Vertex final
	{
		// int16_t data;
		glm::vec3 pos;
	};

	class VOX_SMITH_API Mesh final
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void bakeGreedyMesh(const std::vector<Voxel>& voxels, const glm::vec3& pos);

	private:
		std::vector<Vertex> m_vertices;

		Buffer m_buffer;
		//const std::shared_ptr<Renderer> m_renderer;
	};
}