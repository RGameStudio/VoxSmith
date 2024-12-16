#pragma once

#include <future>
#include <memory>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

#include <VoxSmith/Core.hpp>

namespace VoxSmith
{
	class Renderer;
	class Chunk;
	class Shader;
	class Task;
	struct HeightMap;

	class VOX_SMITH_API World
	{
	public:
		World(const glm::vec3 minBoundary = glm::vec3(0),
			const glm::vec3 maxBoundary = glm::vec3(16, 12, 16));
		World(const glm::vec3& playerPos, const int32_t radiusChunks);
		~World() = default;

		void update(const glm::vec3& playerPos);
		void draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, const glm::vec3& playerPos,
			const float renderDistance, bool isOutlineActive = false);

		World(const World&) = delete;
		World& operator=(const World&) = delete;

	private:
		void notifyChunkNeighbours(const glm::vec3& pos);
		void constructMeshes(std::multimap<float, glm::ivec3> chunksToBake);
		void generateChunks(std::multimap<float, glm::ivec3> chunksToConstruct);

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
				return a.x == b.x && a.y == b.y && a.z == b.z;
			}
		};
	
		std::unordered_map<glm::vec3, std::shared_ptr<Chunk>, KeyFuncs> m_chunks;
		int32_t m_radiusChunk;

		std::shared_ptr<HeightMap> m_heightMap;
#if 1
		std::vector<glm::ivec3> m_chunksToBake;
		bool m_bakingInProcess = false;
		std::future<void> m_bakingTask;

		//std::multimap<glm::ivec3> m_chunksToConstruct;
		bool m_chunksConstructionInPorcess = false;
		std::future<void> m_constructionTask;

		mutable std::shared_mutex m_mutex;
#else
		Task m_constructionTask;
		Task m_bakingTask;
#endif
	};
}