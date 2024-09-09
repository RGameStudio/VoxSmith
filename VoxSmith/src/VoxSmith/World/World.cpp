#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"

#include "World.hpp"

using namespace VoxSmith;

constexpr float g_cSize = 32;

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
{
	m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_baseNoiseGen.SetFractalOctaves(8);
	m_baseNoiseGen.SetFrequency(0.0002f);
	m_baseNoiseGen.SetFractalLacunarity(2.0f);

	m_mountainNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_mountainNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_mountainNoiseGen.SetFractalOctaves(8);
	m_mountainNoiseGen.SetFrequency(0.0019f);
	m_mountainNoiseGen.SetFractalLacunarity(2.4f);

	for (int32_t y = minBoundary.y; y < maxBoundary.y; y += g_cSize)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z += g_cSize)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x += g_cSize)
			{
				auto pos = glm::vec3(x, y, z);

				m_chunkTasks.push(
					std::async(&World::createChunk, this, pos, std::ref(m_baseNoiseGen), std::ref(m_mountainNoiseGen)));

				if (m_chunkTasks.size() >= m_maxThreads)
				{
					auto chunk = m_chunkTasks.front().get();
					m_chunks[chunk.getPos()] = chunk;
					m_meshes.push_back(std::make_shared<Mesh>());
					m_chunks[chunk.getPos()].setMesh(m_meshes.back());
					m_chunkTasks.pop();
				}
			}
		}
	}

	while (!m_chunkTasks.empty())
	{
		auto chunk = m_chunkTasks.front().get();
		m_chunks[chunk.getPos()] = chunk;
		m_meshes.push_back(std::make_shared<Mesh>());
		m_chunks[chunk.getPos()].setMesh(m_meshes.back());
		m_chunkTasks.pop();
	}
}

Chunk World::createChunk(const glm::vec3& pos, FastNoiseLite& base, FastNoiseLite& mountain)
{
	return Chunk(pos, base, mountain);
}

void World::update()
{
	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk.getState() == ChunkState::VOXELS_GENERATED && m_meshTasks.size() < m_maxThreads)
		{
			notifyChunkNeighbours(pos);
			m_meshTasks.emplace_back(
				std::move(std::async(std::launch::async, &World::constructMesh, this, pos)));
		}
	}

	m_meshTasks.erase(
		std::remove_if(m_meshTasks.begin(), m_meshTasks.end(),
			[this](auto& task) {
				if (task.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
				{
					glm::vec3 pos = task.get();
					m_chunks[pos].loadVerticesToBuffer();
					return true;
				}
				return false;
			}),
		m_meshTasks.end());
}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader, bool isOutlineActive)
{
	for (const auto& [pos, chunk] : m_chunks)
	{
		if (chunk.getState() == ChunkState::READY && chunk.getState() != ChunkState::EMPTY)
		{
			chunk.draw(renderer, shader, isOutlineActive);
		}
	}
}

glm::vec3 World::constructMesh(const glm::vec3& pos)
{
	return m_chunks[pos].constructMesh();
}

void World::notifyChunkNeighbours(const glm::vec3& pos)
{
	const auto checkAndAddNeighbour = [this, pos](const glm::vec3& neighbourPos, Direction dir) {
		if (m_chunks.find(neighbourPos) != m_chunks.end())
		{
			m_chunks[pos].addNeighbour(dir, &m_chunks[neighbourPos]);
		}
	};

	checkAndAddNeighbour({ pos.x - g_cSize, pos.y, pos.z }, LEFT);
	checkAndAddNeighbour({ pos.x + g_cSize, pos.y, pos.z }, RIGHT);
	checkAndAddNeighbour({ pos.x, pos.y - g_cSize, pos.z }, BOTTOM);
	checkAndAddNeighbour({ pos.x, pos.y + g_cSize, pos.z }, TOP);
	checkAndAddNeighbour({ pos.x, pos.y, pos.z - g_cSize }, BACK);
	checkAndAddNeighbour({ pos.x, pos.y, pos.z + g_cSize }, FRONT);
}
