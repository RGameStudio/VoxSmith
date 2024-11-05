#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"

#include "World.hpp"

using namespace VoxSmith;

constexpr float g_cSize = 32;

constexpr float g_renderDistance = 12 * g_cSize;
constexpr float g_loadDistance = 16 * g_cSize;

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
{
	m_chunks.reserve(32 * 32 * 8);

	m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_baseNoiseGen.SetFractalOctaves(8);
	m_baseNoiseGen.SetFrequency(0.0007f);
	m_baseNoiseGen.SetFractalLacunarity(2.0f);

	m_mountainNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_mountainNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_mountainNoiseGen.SetFractalOctaves(6);
	m_mountainNoiseGen.SetFrequency(0.00165f);
	m_mountainNoiseGen.SetFractalLacunarity(1.83f);

#define BOUNDARY_TERRAIN true
#ifdef BOUNDARY_TERRAIN
	for (int32_t y = minBoundary.y; y < maxBoundary.y; y += g_cSize)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z += g_cSize)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x += g_cSize)
			{
				auto pos = glm::vec3(x, y, z);

				m_chunks[pos] = std::make_shared<Chunk>(pos);
				m_tasks.emplace_back(
					std::async(&Chunk::generateChunk,
						m_chunks[pos],
						std::ref(m_baseNoiseGen), std::ref(m_mountainNoiseGen)));
				m_meshes.push_back(std::make_shared<Mesh>());
				m_chunks[pos]->setMesh(m_meshes.back());

				if (m_tasks.size() >= m_maxThreads)
				{
					m_tasks.begin()->get();
					m_tasks.erase(m_tasks.begin());
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
	for (int32_t iMesh = 0; iMesh < 12 * 12 * 12; iMesh++)
	{
		m_meshes.emplace_back(std::make_shared<Mesh>());
	}
#endif
}

World::World(const glm::vec3& playerPos)
{
	
}

void World::update(const glm::vec3& playerPos)
{
	for (auto& [pos, chunk] : m_chunks)
	{
		auto state = chunk->getState();
		switch (state)
		{

		case ChunkState::VOXELS_GENERATED: {
			if (m_tasks.size() <= m_maxThreads)
			{
				notifyChunkNeighbours(pos);
				chunk->setState(ChunkState::MESH_BAKING);
				m_tasks.emplace_back(std::async(&Chunk::constructMesh, chunk));
			}
			break;
		}

		case ChunkState::MESH_BAKED: {
			chunk->loadVerticesToBuffer();
			break;
		}

		}
	}

	m_tasks.erase(
		std::remove_if(m_tasks.begin(), m_tasks.end(),
			[](auto& task) {
				return task.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
			}),
		m_tasks.end());
}

void World::loadColumn(const glm::vec3& pos, const int32_t height)
{
	for (int32_t iChunk = 0; iChunk < height; iChunk++)
	{
		
	}
}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, const glm::vec3& playerPos, 
	const float renderDistance, bool isOutlineActive)
{
	for (const auto& [pos, chunk] : m_chunks)
	{
		if (chunk->getState() == ChunkState::READY && glm::distance(playerPos, pos) < renderDistance)
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
