#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Chunk/Voxel.hpp"
#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/World/HeightMap/HeightMap.hpp"
#include "VoxSmith/Timer/Timestep.hpp"

#include "WorldManager.hpp"
#include "World.hpp"

using namespace VoxSmith;

constexpr int32_t g_sAxis = 32;

constexpr float g_renderDistance = 3 * g_sAxis;

namespace UpdateConstants
{
	constexpr uint32_t g_maxChunksToGen = 64;
	constexpr uint32_t g_maxMeshesToConstruct = 64;
}

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
	: m_heightMap(std::make_shared<HeightMap>())
{
	m_chunks.reserve(32 * 32 * 8);
	WorldManager::getInstance().init(this);

	for (int32_t y = minBoundary.y; y < maxBoundary.y; y++)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z++)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x++)
			{
				const glm::vec3 pos = { x * g_sAxis, y * g_sAxis, z * g_sAxis };
				m_chunks[pos] = std::make_shared<Chunk>(pos);
			}
		}
	}
}

World::World(const glm::vec3& playerPos, const int32_t radiusChunk)
	: m_heightMap(std::make_shared<HeightMap>())
	, m_radiusChunk(radiusChunk)
{
	WorldManager::getInstance().init(this);
	m_generation.function = std::bind(&World::generateChunks, this);
	m_baking.function = std::bind(&World::bakeMeshes, this);
}

void World::traverseChunks(const glm::ivec3& initPos, const glm::ivec3& endPos)
{
	for (int32_t y = initPos.y; y < endPos.y; y += g_sAxis)
	{
		for (int32_t z = initPos.z; z < endPos.z; z += g_sAxis)
		{
			for (int32_t x = initPos.x; x < endPos.x; x += g_sAxis)
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
		static_cast<int32_t>(playerPos.x / g_sAxis) * g_sAxis,
		static_cast<int32_t>(playerPos.z / g_sAxis) * g_sAxis,
		static_cast<int32_t>(playerPos.z / g_sAxis) * g_sAxis,
	};
	const glm::ivec3 initPos = {
		playerChunkPos.x - m_radiusChunk * g_sAxis,
		0,
		playerChunkPos.z - m_radiusChunk * g_sAxis
	};
	const glm::ivec3 endPos = {
		playerChunkPos.x + m_radiusChunk * g_sAxis,
		m_radiusChunk * g_sAxis,
		playerChunkPos.z + m_radiusChunk * g_sAxis
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

	updateChunkTask(m_generation, m_chunksToGenerate);
	updateChunkTask(m_baking, m_chunksToBake);
}

void World::updateChunkTask(TaskWrapper& tWrapper, 
	std::vector<std::shared_ptr<Chunk>>& chunkList)
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
				m_chunks[chunk->getPos()] = std::move(chunk);
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

	checkAndAddNeighbour(pos, { pos.x - g_sAxis, pos.y, pos.z }, LEFT);
	checkAndAddNeighbour(pos, { pos.x + g_sAxis, pos.y, pos.z }, RIGHT);
	checkAndAddNeighbour(pos, { pos.x, pos.y - g_sAxis, pos.z }, BOTTOM);
	checkAndAddNeighbour(pos, { pos.x, pos.y + g_sAxis, pos.z }, TOP);
	checkAndAddNeighbour(pos, { pos.x, pos.y, pos.z - g_sAxis }, BACK);
	checkAndAddNeighbour(pos, { pos.x, pos.y, pos.z + g_sAxis }, FRONT);

	checkAndAddNeighbour(pos, {pos.x - g_sAxis, pos.y + g_sAxis, pos.z - g_sAxis}, TOP_LEFT_BACK);
	checkAndAddNeighbour(pos, {pos.x - g_sAxis, pos.y + g_sAxis, pos.z + g_sAxis}, TOP_LEFT_FRONT);
	checkAndAddNeighbour(pos, {pos.x + g_sAxis, pos.y + g_sAxis, pos.z - g_sAxis}, TOP_RIGHT_BACK);
	checkAndAddNeighbour(pos, {pos.x + g_sAxis, pos.y + g_sAxis, pos.z + g_sAxis}, TOP_RIGHT_FRONT);

	checkAndAddNeighbour(pos, {pos.x - g_sAxis, pos.y - g_sAxis, pos.z - g_sAxis}, BOTTOM_LEFT_BACK);
	checkAndAddNeighbour(pos, {pos.x - g_sAxis, pos.y - g_sAxis, pos.z + g_sAxis}, BOTTOM_LEFT_FRONT);
	checkAndAddNeighbour(pos, {pos.x + g_sAxis, pos.y - g_sAxis, pos.z - g_sAxis}, BOTTOM_RIGHT_BACK);
	checkAndAddNeighbour(pos, {pos.x + g_sAxis, pos.y - g_sAxis, pos.z + g_sAxis}, BOTTOM_RIGHT_FRONT);

	checkAndAddNeighbour(pos, {pos.x + g_sAxis, pos.y, pos.z + g_sAxis}, FRONT_RIGHT);
	checkAndAddNeighbour(pos, {pos.x - g_sAxis, pos.y, pos.z + g_sAxis}, FRONT_LEFT);
	checkAndAddNeighbour(pos, {pos.x + g_sAxis, pos.y, pos.z - g_sAxis}, BACK_RIGHT);
	checkAndAddNeighbour(pos, {pos.x - g_sAxis, pos.y, pos.z - g_sAxis}, BACK_LEFT);

	checkAndAddNeighbour(pos, {pos.x, pos.y + g_sAxis, pos.z + g_sAxis}, TOP_FRONT);
	checkAndAddNeighbour(pos, {pos.x, pos.y + g_sAxis, pos.z - g_sAxis}, TOP_BACK);
	checkAndAddNeighbour(pos, {pos.x, pos.y - g_sAxis, pos.z + g_sAxis}, BOTTOM_FRONT);
	checkAndAddNeighbour(pos, {pos.x, pos.y - g_sAxis, pos.z - g_sAxis}, BOTTOM_BACK);
}

VoxelType World::getVoxel(const glm::vec3& pos) const
{
	glm::vec3 pChunk = glm::vec3(
		static_cast<int32_t>(pos.x) / g_sAxis,
		static_cast<int32_t>(pos.y) / g_sAxis,
		static_cast<int32_t>(pos.z) / g_sAxis
	);

	glm::vec3 pVoxel = glm::vec3(
		static_cast<int32_t>(pos.x) % g_sAxis,
		static_cast<int32_t>(pos.y) % g_sAxis,
		static_cast<int32_t>(pos.z) % g_sAxis
	);

	const auto p = pChunk;

	pChunk.x = pVoxel.x < 0 ? pChunk.x - 1 : pChunk.x;
	pChunk.y = pVoxel.y < 0 ? pChunk.y - 1 : pChunk.y;
	pChunk.z = pVoxel.z < 0 ? pChunk.z - 1 : pChunk.z;

	pChunk *= g_sAxis;

	const glm::vec3 pV = pVoxel;

	pVoxel.x = pVoxel.x < 0 ? 32 + pVoxel.x : pVoxel.x;
	pVoxel.y = pVoxel.y < 0 ? 32 + pVoxel.y : pVoxel.y;
	pVoxel.z = pVoxel.z < 0 ? 32 + pVoxel.z : pVoxel.z;
	
	if (m_chunks.find(pChunk) == m_chunks.end() || m_chunks.at(pChunk) == nullptr)
	{
		return VoxelType::Empty;
	}

	return m_chunks.at(pChunk)->getVoxel(pVoxel);
}
