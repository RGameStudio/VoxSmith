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
			posData |= (pos.x & 0x1F);
			posData |= (pos.y & 0x1F) << 5;
			posData |= (pos.z & 0x1F) << 10;

			auto test = {
				posData & 31,
				(posData >> 5) & 31,
				(posData >> 10) & 31,
			};

			texColLightData |= (color.r & 0b11111111);
			texColLightData |= (color.g & 0b11111111) << 8;
			texColLightData |= (color.b & 0b11111111) << 16;
			
			test = {
				texColLightData & 255,
				(texColLightData >> 8) & 255,
				((texColLightData) >> 16) & 255
			};

			texColLightData |= (id & 0b11111111) << 24;
		}

		Vertex(const glm::ivec3& pos, const uint8_t texId, const uint8_t uvId)
		{
			posData |= (pos.x & 0b11111);
			posData |= (pos.y & 0b11111) << 5;
			posData |= (pos.z & 0b11111) << 10;

			// texColLightData |= (texId & 0b11111111);
			// texColLightData |= (uvId & 0b11111111) << uvId;
		}

		int32_t posData = 0;
		int32_t texColLightData = 0;
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