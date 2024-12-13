#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/World/HeightMap/HeightMap.hpp"

#include "World.hpp"

using namespace VoxSmith;

constexpr int32_t g_cSize = 32;

constexpr float g_renderDistance = 12 * g_cSize;
constexpr float g_loadDistance = 16 * g_cSize;

namespace UpdateConstants
{
	constexpr uint32_t g_maxChunksToGen = 8;
	constexpr uint32_t g_maxMeshesToConstruct = 4;
}

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
	: m_heightMap(std::make_shared<HeightMap>())
{
	m_chunks.reserve(32 * 32 * 8);

	for (int32_t y = minBoundary.y; y < maxBoundary.y; y++)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z++)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x++)
			{
				const glm::vec3 pos = { x * g_cSize, y * g_cSize, z * g_cSize };
				m_chunks[pos] = std::make_shared<Chunk>(pos);
#if 1
				m_chunksToConstruct.push(pos);
#else
				m_chunks[pos]->generateChunk(m_heightMap->getChunkMap({ pos.x, pos.z }));
#endif
			}
		}
	}
}

World::World(const glm::vec3& playerPos, const int32_t radiusChunk)
	: m_heightMap(std::make_shared<HeightMap>())
{
#if 1
	const glm::vec3 playerChunkPos = {
		static_cast<int32_t>(playerPos.x / g_cSize) * g_cSize,
		static_cast<int32_t>(playerPos.y / g_cSize) * g_cSize,
		static_cast<int32_t>(playerPos.z / g_cSize) * g_cSize,
	};

	const glm::ivec3 initPos = static_cast<glm::ivec3>(playerChunkPos) - radiusChunk * glm::ivec3(g_cSize);
	const glm::ivec3 endPos = static_cast<glm::ivec3>(playerChunkPos) + radiusChunk * glm::ivec3(g_cSize);

	for (int32_t y = initPos.y; y < endPos.y; y += g_cSize)
	{
		for (int32_t z = initPos.z; z < endPos.z; z += g_cSize)
		{
			for (int32_t x = initPos.x; x < endPos.x; x += g_cSize)
			{
				const glm::ivec3 pos = { x, y, z };
				m_chunks[pos] = std::make_shared<Chunk>(pos);
				m_chunksToConstruct.push(pos);
			}
		}
	}
#else
	glm::ivec3 startPos = playerPos - static_cast<float>(radiusChunk) * glm::vec3(g_cSize);
	glm::ivec3 endPos = playerPos + static_cast<float>(radiusChunk) * glm::vec3(g_cSize);

	startPos = {
		static_cast<int32_t>(startPos.x / g_cSize) * g_cSize,
		static_cast<int32_t>(startPos.y / g_cSize) * g_cSize,
		static_cast<int32_t>(startPos.z / g_cSize) * g_cSize,
	};
	endPos = {
		static_cast<int32_t>(endPos.x / g_cSize) * g_cSize,
		static_cast<int32_t>(endPos.y / g_cSize) * g_cSize,
		static_cast<int32_t>(endPos.z / g_cSize) * g_cSize,
	};
#endif

#if 0
	for (int32_t y = startPos.y; y < endPos.y; y += g_cSize)
	{
		for (int32_t z = startPos.z; z < endPos.z; z += g_cSize)
		{
			for (int32_t x = startPos.x; x < endPos.x; x += g_cSize)
			{
				glm::vec3 pos = { x, y, z };

				m_chunks[pos] = std::make_shared<Chunk>(pos);
				m_tasks.emplace_back(
					std::async(&Chunk::generateChunk,
						m_chunks[pos],
						std::ref(m_baseNoiseGen), std::ref(m_mountainNoiseGen)));
				m_meshes.push_back(std::make_shared<Mesh>());
				m_chunks[pos]->setMesh(m_meshes.back());

				if (m_tasks.size() >= m_maxThreads)
				{
					while (!m_tasks.empty())
					{
						m_tasks.begin()->get();
						m_tasks.erase(m_tasks.begin());
					}
				}
			}
		}
	}

	while (!m_tasks.empty())
	{
		m_tasks.begin()->get();
		m_tasks.erase(m_tasks.begin());
	}
#else
#endif
}

void World::update(const glm::vec3& playerPos)
{
	if (!m_chunksConstructionInPorcess && !m_chunksToConstruct.empty())
	{
		m_constructionTask = std::move(std::async(&World::generateChunks, this));
		m_chunksConstructionInPorcess = true;
	}
	else if (m_chunksConstructionInPorcess &&
		m_constructionTask.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		m_chunksConstructionInPorcess = false;
		m_constructionTask.get();
	}

	std::vector<glm::ivec3> chunksToBake;
	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk == nullptr)
		{
			continue;
		}

		auto state = chunk->getState();
		switch (state)
		{

		case ChunkState::VOXELS_GENERATED: {
			notifyChunkNeighbours(pos);
			if (chunk->canBake() && m_tasks.size() < m_maxThreads)
			{
				//std::lock_guard<std::mutex> lock(m_mutex);
				//m_tasks.emplace_back(std::async(&Chunk::constructMesh, chunk));
				chunksToBake.push_back(pos);
			}
			break;
		}

		case ChunkState::MESH_BAKED: {
			chunk->loadVerticesToBuffer();
			break;
		}

		}
	}

	if (!m_bakingInProcess && !chunksToBake.empty())
	{
		m_bakingTask = std::move(std::async(&World::constructMeshes, this, std::move(chunksToBake)));
		m_bakingInProcess = true;
	}
	else if (m_bakingInProcess &&
		m_bakingTask.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		m_bakingInProcess = false;
		m_bakingTask.get();
	}
}

void World::generateChunks()
{
	int32_t cCounter = 0;
	while (!m_chunksToConstruct.empty() &&
		cCounter < UpdateConstants::g_maxChunksToGen)
	{
		const glm::ivec3& pos = m_chunksToConstruct.front();
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_chunks[pos]->generateChunk(m_heightMap->getChunkMap({ pos.x, pos.z }));
			m_chunksToConstruct.pop();
		}
		cCounter++;
	}
}

void World::constructMeshes(std::vector<glm::ivec3> chunksToBake)
{
	for (const auto& pos : chunksToBake)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_chunks[pos]->setState(ChunkState::MESH_BAKING);
		m_chunks[pos]->constructMesh();
	}
}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, const glm::vec3& playerPos,
	const float renderDistance, bool isOutlineActive)
{
	for (const auto& [pos, chunk] : m_chunks)
	{
		if (chunk->getState() == ChunkState::READY &&
			glm::distance(playerPos, pos) < renderDistance)
		{
			chunk->draw(renderer, shader, isOutlineActive);
		}
	}
}

void World::notifyChunkNeighbours(const glm::vec3& pos)
{
	const auto checkAndAddNeighbour = [this](const glm::vec3& pos, const glm::vec3& neighbourPos, Direction dir) {
		if (m_chunks.find(neighbourPos) != m_chunks.end() &&
			m_chunks.find(pos) != m_chunks.end())
		{
			m_chunks[pos]->addNeighbour(dir, m_chunks[neighbourPos]);
			// m_chunks[neighbourPos]->addNeighbour(getInverseDirection(dir), m_chunks[pos]);
		}
	};

	checkAndAddNeighbour(pos, { pos.x - g_cSize, pos.y, pos.z }, LEFT);
	checkAndAddNeighbour(pos, { pos.x + g_cSize, pos.y, pos.z }, RIGHT);
	checkAndAddNeighbour(pos, { pos.x, pos.y - g_cSize, pos.z }, BOTTOM);
	checkAndAddNeighbour(pos, { pos.x, pos.y + g_cSize, pos.z }, TOP);
	checkAndAddNeighbour(pos, { pos.x, pos.y, pos.z - g_cSize }, BACK);
	checkAndAddNeighbour(pos, { pos.x, pos.y, pos.z + g_cSize }, FRONT);
}
