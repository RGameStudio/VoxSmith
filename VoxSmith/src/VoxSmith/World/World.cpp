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

namespace UpdateConstants
{
	constexpr uint32_t g_maxChunksToGen = 8;
	constexpr uint32_t g_maxMeshesToConstruct = 8;
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
				//m_chunksToConstruct.insert(pos);
#else
				m_chunks[pos]->generateChunk(m_heightMap->getChunkMap({ pos.x, pos.z }));
#endif
			}
		}
	}
}

World::World(const glm::vec3& playerPos, const int32_t radiusChunk)
	: m_heightMap(std::make_shared<HeightMap>())
	, m_radiusChunk(radiusChunk)
{
#if 0
	const glm::vec3 playerChunkPos = {
		static_cast<int32_t>(playerPos.x / g_cSize) * g_cSize,
		static_cast<int32_t>(playerPos.y / g_cSize) * g_cSize,
		static_cast<int32_t>(playerPos.z / g_cSize) * g_cSize,
	};

	const glm::ivec3 initPos = static_cast<glm::ivec3>(playerChunkPos) - m_radiusChunk * glm::ivec3(g_cSize);
	const glm::ivec3 endPos = static_cast<glm::ivec3>(playerChunkPos) + m_radiusChunk * glm::ivec3(g_cSize);

	for (int32_t y = initPos.y; y < endPos.y; y += g_cSize)
	{
		for (int32_t z = initPos.z; z < endPos.z; z += g_cSize)
		{
			for (int32_t x = initPos.x; x < endPos.x; x += g_cSize)
			{
				const glm::ivec3 pos = { x, y, z };
				m_chunks[pos] = std::make_shared<Chunk>(pos);
				m_chunksToConstruct.push_back(pos);
			}
		}
	}
#endif
}

void World::update(const glm::vec3& playerPos)
{
#if 1
	const glm::ivec3 playerChunkPos = {
		static_cast<int32_t>(playerPos.x / g_cSize) * g_cSize,
		static_cast<int32_t>(playerPos.z / g_cSize) * g_cSize,
		static_cast<int32_t>(playerPos.z / g_cSize) * g_cSize,
	};
	const glm::ivec3 initPos = {
		playerChunkPos.x - m_radiusChunk * g_cSize,
		0,
		playerChunkPos.z - m_radiusChunk * g_cSize
	};
	const glm::ivec3 endPos = {
		playerChunkPos.x + m_radiusChunk * g_cSize,
		2 * m_radiusChunk * g_cSize,
		playerChunkPos.z + m_radiusChunk * g_cSize
	};

	std::multimap<float, glm::ivec3> chunksToConstruct;
	for (int32_t y = initPos.y; y < endPos.y; y += g_cSize)
	{
		for (int32_t z = initPos.z; z < endPos.z; z += g_cSize)
		{
			for (int32_t x = initPos.x; x < endPos.x; x += g_cSize)
			{
				const glm::ivec3 pos = { x, y, z };
				const float distance = glm::distance(playerPos, static_cast<glm::vec3>(pos));
				if (chunksToConstruct.size() < UpdateConstants::g_maxChunksToGen)
				{
					if (m_chunks.find(pos) == m_chunks.end())
					{
						m_chunks[pos] = std::make_shared<Chunk>(pos);
						chunksToConstruct.insert({ distance, pos });
					}
					else if (m_chunks[pos]->getState() == ChunkState::EMPTY)
					{
						chunksToConstruct.insert({ distance, pos });
					}
				}
			}
		}
	}
#endif
	std::multimap<float, glm::ivec3> chunksToBake;
	std::vector<glm::ivec3> chunksToRemove;
	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk == nullptr)
		{
			continue;
		}

		if (!chunk->inBounds(initPos, endPos))
		{
			const float distance = glm::distance(playerPos, pos);
			// chunksToConstruct.erase(chunksToConstruct.find({ distance }));
			chunksToRemove.push_back(pos);
			continue;
		}

		auto state = chunk->getState();
		switch (state)
		{

		case ChunkState::VOXELS_GENERATED: {
			notifyChunkNeighbours(pos);
			if (chunk->canBake())
			{
				const float distance = glm::distance(pos, playerPos);
				chunksToBake.insert({ distance, pos });
			}
			break;
		}

		case ChunkState::MESH_BAKED: {
			chunk->loadVerticesToBuffer();
			break;
		}
		case ChunkState::READY: {
			break;
		}
		}
	}

#if 1
	{
		for (auto& pos : chunksToRemove)
		{
			std::lock_guard<std::shared_mutex> lock(m_mutex);
			m_chunks.erase(pos);
		}
	}
#endif
	if (!m_chunksConstructionInPorcess && !chunksToConstruct.empty())
	{
		m_constructionTask = std::move(std::async(&World::generateChunks, this, std::move(chunksToConstruct)));
		m_chunksConstructionInPorcess = true;
	}
	else if (m_chunksConstructionInPorcess &&
		m_constructionTask.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		m_chunksConstructionInPorcess = false;
		try
		{
			m_constructionTask.get();
		}
		catch (std::exception& e)
		{
			LOG_CORE_ERROR("In construction thread: {0}, {1}", e.what(), m_constructionTask.valid());
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
		try
		{
			m_bakingTask.get();
		}
		catch (std::exception& e)
		{
			LOG_CORE_ERROR(e.what());
		}
		m_bakingInProcess = false;
	}
}

void World::generateChunks(std::multimap<float, glm::ivec3> chunksToConstruct)
{
	for (const auto& [distance, pos] : chunksToConstruct)
	{
		std::lock_guard<std::shared_mutex> lock(m_mutex);
		if (m_chunks.find(pos) != m_chunks.end())
		{
			m_chunks[pos]->generateChunk(m_heightMap->getChunkMap({ pos.x, pos.z }));
		}
	}
}

void World::constructMeshes(std::multimap<float, glm::ivec3> chunksToBake)
{
	for (const auto& [distance, pos] : chunksToBake)
	{
		std::lock_guard<std::shared_mutex> lock(m_mutex);
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
			glm::distance(pos, playerPos) < renderDistance)
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
		}
	};

	checkAndAddNeighbour(pos, { pos.x - g_cSize, pos.y, pos.z }, LEFT);
	checkAndAddNeighbour(pos, { pos.x + g_cSize, pos.y, pos.z }, RIGHT);
	checkAndAddNeighbour(pos, { pos.x, pos.y - g_cSize, pos.z }, BOTTOM);
	checkAndAddNeighbour(pos, { pos.x, pos.y + g_cSize, pos.z }, TOP);
	checkAndAddNeighbour(pos, { pos.x, pos.y, pos.z - g_cSize }, BACK);
	checkAndAddNeighbour(pos, { pos.x, pos.y, pos.z + g_cSize }, FRONT);
}
