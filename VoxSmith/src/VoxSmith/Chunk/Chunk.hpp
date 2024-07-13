#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Voxel.hpp"

#include "../Core.hpp"

namespace VoxSmith
{
	class Renderer;
	class Shader;

	class VOX_SMITH_API Chunk final
	{
	public:
		Chunk(const glm::vec3& pos);
		~Chunk() = default;

		void setMesh(const std::shared_ptr<Mesh>& mesh);
		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const;
		void constructMesh();

	private:
		glm::vec3 m_pos;

		std::shared_ptr<Mesh> m_mesh = nullptr;
		std::vector<Voxel> m_voxels;
	};
}