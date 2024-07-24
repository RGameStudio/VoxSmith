#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "../Chunk/Voxel.hpp"

#include "Buffer.hpp"
#include "../Core.hpp"

namespace VoxSmith
{
	class Renderer;
	class Shader;
	class Buffer;

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

		void loadToBuffer(const std::vector<Vertex>& vertices);

		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const;

	private:
		Buffer m_buffer;

		uint32_t m_vertexCount = 0;
	};
}