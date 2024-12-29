#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/World/HeightMap/HeightMap.hpp"
#include "VoxSmith/Timer/Timestep.hpp"

#include "World.hpp"

using namespace VoxSmith;

constexpr int32_t g_cSize = 32;

constexpr float g_renderDistance = 12 * g_cSize;

namespace UpdateConstants
{
	constexpr uint32_t g_maxChunksToGen = 64;
	constexpr uint32_t g_maxMeshesToConstruct = 32;
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
			}
		}
	}
}

World::World(const glm::vec3& playerPos, const int32_t radiusChunk)
	: m_heightMap(std::make_shared<HeightMap>())
	, m_radiusChunk(radiusChunk)
{
	m_generation.function = std::bind(&World::generateChunks, this);
	m_baking.function = std::bind(&World::bakeMeshes, this);
}

void World::traverseChunks(const glm::ivec3& initPos, const glm::ivec3& endPos)
{
	for (int32_t y = initPos.y; y < endPos.y; y += g_cSize)
	{
		for (int32_t z = initPos.z; z < endPos.z; z += g_cSize)
		{
			for (int32_t x = initPos.x; x < endPos.x; x += g_cSize)
			{
				const glm::ivec3 pos = { x, y, z };

				if (m_chunks.find(pos) == m_chunks.end())
				{
					m_chunks[pos] = std::make_shared<Chunk>(pos);
				}

				if (m_chunksToGenerate.size() < UpdateConstants::g_maxChunksToGen)
				{
					if (m_chunks[pos] != nullptr && m_chunks[pos]->getState() == ChunkState::EMPTY)
					{
						m_chunksToGenerate.push_back(std::move(m_chunks[pos]));
					}
				}
			}
		}
	}
}

void World::update(const glm::vec3& playerPos)
{
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
		m_radiusChunk * g_cSize,
		playerChunkPos.z + m_radiusChunk * g_cSize
	};

	traverseChunks(initPos, endPos);

	std::vector<glm::ivec3> chunksToRemove;
	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk == nullptr)
		{
			continue;
		}

		if (!chunk->inBounds(initPos, endPos))
		{
			chunksToRemove.push_back(pos);
			continue;
		}

		auto state = chunk->getState();
		switch (state)
		{

		case ChunkState::VOXELS_GENERATED: {
			notifyChunkNeighbours(pos);
			if (chunk->canBake() && !m_baking.launched && m_chunksToBake.size() < UpdateConstants::g_maxMeshesToConstruct)
			{
				const float distance = glm::distance(pos, playerPos);
				m_chunksToBake.push_back(std::move(chunk));
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

	for (auto& pos : chunksToRemove)
	{
		m_chunks.erase(pos);
	}

	updateChunkTask(m_generation, m_chunksToGenerate, initPos, endPos);
	updateChunkTask(m_baking, m_chunksToBake, initPos, endPos);
}

void World::updateChunkTask(TaskWrapper& tWrapper, std::vector<std::shared_ptr<Chunk>>& chunkList, const glm::ivec3& initPos, const glm::ivec3& endPos)
{
	if (!tWrapper.launched && !chunkList.empty())
	{
		tWrapper.task = std::move(std::async(tWrapper.function));
		tWrapper.launched = true;
	}
	else if (tWrapper.launched && tWrapper.completed())
	{
		try
		{
			tWrapper.get();
			for (auto& chunk : chunkList)
			{
				if (chunk->inBounds(initPos, endPos))
				{
					m_chunks[chunk->getPos()] = std::move(chunk);
				}
			}
			chunkList.clear();
			tWrapper.launched = false;
		}
		catch (std::exception& e)
		{
			LOG_CORE_ERROR("In construction thread: {0}, {1}", e.what(), tWrapper.task.valid());
		}
	}
}

void World::generateChunks()
{
	for (auto& chunk : m_chunksToGenerate)
	{
		const auto& pos = chunk->getPos();
		chunk->generateChunk(m_heightMap->getChunkMap({ pos.x, pos.z }));
	}
}

void World::bakeMeshes()
{
	for (auto& chunk : m_chunksToBake)
	{
		chunk->setState(ChunkState::MESH_BAKING);
		chunk->bake();
	}
}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, const glm::vec3& playerPos,
	const Frustum& frustum, bool isOutlineActive)
{
	for (const auto& [pos, chunk] : m_chunks)
	{
		if (chunk == nullptr)
		{
			continue;
		}

		if (chunk->getState() == ChunkState::READY && chunk->inFrustum(frustum))
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
