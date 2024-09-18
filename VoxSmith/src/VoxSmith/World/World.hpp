#pragma once

#include <future>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <FastNoiseLite.h>

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
			const glm::vec3 maxBoundary = glm::vec3(32 * 16, 32 * 8, 32 * 16));
		~World() = default;

		void update();
		void draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, const glm::vec3& playerPos, const float viewDistance, bool isOutlineActive = false);

		World(const World&) = delete;
		World& operator=(const World&) = delete;

	private:
		void notifyChunkNeighbours(const glm::vec3& pos);
		std::shared_ptr<Chunk> createChunk(const glm::vec3& pos, FastNoiseLite& base, FastNoiseLite& mountain);
		glm::vec3 constructMesh(const glm::vec3 pos);

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

		std::unordered_map<glm::vec3, std::shared_ptr<Chunk>, KeyFuncs> m_chunks;
		std::vector<std::shared_ptr<Mesh>> m_meshes;

		uint32_t m_maxThreads = 5;

		FastNoiseLite m_baseNoiseGen;
		FastNoiseLite m_mountainNoiseGen;
		
		std::vector<std::future<glm::vec3>> m_meshTasks;
		std::vector<std::future<std::shared_ptr<Chunk>>> m_chunkTasks;
	};
}