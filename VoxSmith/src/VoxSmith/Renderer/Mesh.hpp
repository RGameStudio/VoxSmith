#pragma once

#include <memory>
#include <vector>
#include <array>
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
		Vertex(const glm::ivec3& pos, const glm::u8vec3& color, const uint8_t id)
		{
			portion1 |= (pos.x & 0b00111111);
			portion1 |= (pos.y & 0b00111111) << 6;
			portion1 |= (pos.z & 0b00111111) << 12;
			
			portion1 |= (color.r & 0b11111111) << 18;
			
			portion1 |= (color.g & 0b00111111) << 26;
			portion2 |= (color.g & 0b11000000) >> 6;
			
			portion2 |= (color.b & 0b11111111) << 2;

			portion2 |= (id & 0b11111111) << 10;
		}

		Vertex(const glm::ivec3& pos, const uint8_t texId, const uint8_t uvId)
		{
			portion1 |= (pos.x & 0b111111);
			portion1 |= (pos.y & 0b111111) << 6;
			portion1 |= (pos.z & 0b111111) << 12;

			portion2 |= (texId & 0b11111111);
			portion2 |= (uvId & 0b11111111) << uvId;
		}

		uint32_t portion1 = 0;
		uint32_t portion2 = 0;
	};

	class VOX_SMITH_API Mesh final
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		template <typename T>
		void loadToBuffer(const std::vector<T>& vertices);

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

	template <typename T>
	void Mesh::loadToBuffer(const std::vector<T>& data)
	{
		m_vertexCount = data.size();
		m_buffer.init(data);
		m_isConstructed = true;
	}
}