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
		Vertex(const glm::vec3& pos_, const glm::vec3& color_, const int32_t id_) 
			: pos(pos_)
			, color(color_)
			, id(id_)
		{

		}

		// int16_t data;
		glm::vec3 pos;
		glm::vec3 color;
		int32_t id;
	};

	class VOX_SMITH_API Mesh final
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void loadToBuffer(const std::vector<Vertex>& vertices);
		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const;

		inline void reserveMesh() { m_isFree = false; }

		inline bool isMeshConstructed() const { return m_isConstructed; }
		inline bool isFree() const { return m_isFree; }

	private:
		Buffer m_buffer;

		bool m_isFree = true;
		bool m_isConstructed = false;

		uint32_t m_vertexCount = 0;
	};
}