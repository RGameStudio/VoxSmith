#pragma once

#include <future>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>

#include <VoxSmith/Core.hpp>

namespace VoxSmith
{
	class Renderer;
	class Chunk;
	class Shader;
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
		void loadColumn(const glm::vec3& pos, const int32_t height);
		void constructMeshes(std::vector<glm::ivec3> chunksToBake);
		void generateChunks();

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

		std::shared_ptr<HeightMap> m_heightMap;
		
		std::vector<glm::ivec3> m_chunksToBake;
		bool m_bakingInProcess = false;
		std::future<void> m_bakingTask;

		std::queue<glm::ivec3> m_chunksToConstruct;
		bool m_chunksConstructionInPorcess = false;
		std::future<void> m_constructionTask;

		const uint32_t m_maxThreads = 6;

		mutable std::mutex m_mutex;

		std::vector<std::future<void>> m_tasks;
	};
}