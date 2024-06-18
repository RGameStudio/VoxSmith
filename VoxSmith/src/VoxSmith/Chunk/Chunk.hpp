#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../Core.hpp"

namespace VoxSmith
{
	class Mesh;
	struct Voxel;

	class VOX_SMITH_API Chunk final
	{
	public:
		Chunk(const glm::vec3& pos);
		~Chunk() = default;

		void draw();
		void constructMesh();

	private:
		glm::vec3 m_pos;

		std::shared_ptr<Mesh> m_mesh;
		std::vector<Voxel> m_voxels;
	};
}