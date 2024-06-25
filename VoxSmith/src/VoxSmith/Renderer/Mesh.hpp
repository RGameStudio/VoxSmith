#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Buffer.hpp"
#include "../Core.hpp"

namespace VoxSmith
{
	class Renderer;
	class Shader;

	struct Voxel;

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

		void loadToBuffer();
		void bakeGreedy(const std::vector<Voxel>& voxels, const glm::vec3& pos, const glm::vec3& size);
		void bakeBinaryGreedy(const std::vector<Voxel>& voxels, const glm::vec3& pos, const glm::vec3& size);
		void draw(const Renderer& renderer, const Shader& shader) const;

	private:
		std::vector<Vertex> m_vertices;

		Buffer m_buffer;
		//const std::shared_ptr<Renderer> m_renderer;
	};
}