#pragma once

#include <future>
#include <memory>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

#include "VoxSmith/Core.hpp"

#include "VoxSmith/Task/Task.hpp"

namespace VoxSmith
{
	class Renderer;
	class Chunk;
	class Shader;
	struct HeightMap;
	struct Frustum;

	class VOX_SMITH_API World
	{
	public:
		World(const glm::vec3 minBoundary = glm::vec3(0),
			const glm::vec3 maxBoundary = glm::vec3(32, 8, 32));
		World(const glm::vec3& playerPos, const int32_t radiusChunks);
		~World() = default;

		void update(const glm::vec3& playerPos);
		void draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, const glm::vec3& playerPos,
			const Frustum& frustum, bool isOutlineActive = false);

		World(const World&) = delete;
		World& operator=(const World&) = delete;

	private:
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

		struct TaskWrapper
		{
			std::future<void> task;
			bool launched = false;
			std::function<void()> function;

			bool completed() const
			{
				return task.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
			}

			void get()
			{
				task.get();
			}
		};

		std::unordered_map<glm::vec3, std::shared_ptr<Chunk>, KeyFuncs> m_chunks;
		std::vector<std::shared_ptr<Chunk>> m_chunksToBake;
		std::vector<std::shared_ptr<Chunk>> m_chunksToGenerate;
		int32_t m_radiusChunk;

		std::shared_ptr<HeightMap> m_heightMap;

		TaskWrapper m_baking;
		TaskWrapper m_generation;
		mutable std::shared_mutex m_mutex;

		void notifyChunkNeighbours(const glm::vec3& pos);
		void bakeMeshes();
		void generateChunks();
		void updateChunkTask(TaskWrapper& task, std::vector<std::shared_ptr<Chunk>>& chunkList,
			const glm::ivec3& initPos, const glm::ivec3& endPos);
		void traverseChunks(const glm::ivec3& initPos, const glm::ivec3& endPos);
	};
}