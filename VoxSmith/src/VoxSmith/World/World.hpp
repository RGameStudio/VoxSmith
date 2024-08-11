#pragma once

#include <memory>
#include <unordered_map>

#include <VoxSmith/Core.hpp>

namespace VoxSmith
{
	class Renderer;
	class Chunk;
	class Shader;

	class VOX_SMITH_API World
	{
	public:
		World(const glm::vec3 minBoundary = glm::vec3(0.0f), 
			const glm::vec3 maxBoundary = glm::vec3(32 * 2, 32, 32 * 2));
		~World();

		void update();
		void draw(std::shared_ptr<Renderer>& renderer, const Shader& shader);

	private:
		void notifyChunkNeighbours(const glm::vec3& pos);

		struct KeyFuncs
		{
			size_t operator()(const glm::ivec3& v) const
			{
				size_t h = 0xcf234123f;
				h ^= std::_Bit_cast<int32_t>(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
				h ^= std::_Bit_cast<int32_t>(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
				h ^= std::_Bit_cast<int32_t>(v.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
				return h;
			}

			bool operator()(const glm::ivec3& a, const glm::ivec3& b) const
			{
				return a.x == b.x && a.z == b.z;
			}
		};

		std::unordered_map<glm::vec3, Chunk, KeyFuncs> m_chunks;
		std::vector<std::shared_ptr<Mesh>> m_meshes;
	};
}