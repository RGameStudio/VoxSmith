#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"

#include "World.hpp"

using namespace VoxSmith;

constexpr float g_cSize = 32;

std::vector<std::future<glm::vec3>> m_tasks;

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
{
	m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_baseNoiseGen.SetFractalOctaves(8);
	m_baseNoiseGen.SetFrequency(0.0002f);
	m_baseNoiseGen.SetFractalLacunarity(2.0f);

	m_mountainNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_mountainNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_mountainNoiseGen.SetFractalOctaves(6);
	m_mountainNoiseGen.SetFrequency(0.0017f);
	m_mountainNoiseGen.SetFractalLacunarity(2.0f);

	for (int32_t y = minBoundary.y; y < maxBoundary.y; y += g_cSize)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z += g_cSize)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x += g_cSize)
			{
				auto pos = glm::vec3(x, y, z);
				m_meshes.push_back(std::make_shared<Mesh>());

				m_chunks[pos] = std::move(Chunk(pos, m_baseNoiseGen, m_mountainNoiseGen));
				m_chunks[pos].setMesh(m_meshes.back());
			}
		}
	}
}

World::~World()
{

}

void World::update()
{
	m_tasks.erase(
		std::remove_if(
			m_tasks.begin(),
			m_tasks.end(),
			[this](auto& task) {
				if (task.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
				{
					auto pos = task.get();
					auto& chunk = m_chunks[pos];
					//if (chunk.getState() == ChunkState::MESH_BAKED)
					{
						m_chunks[pos].loadVerticesToBuffer();
						return true;
					}
				}
				return false;
			}
		),
		m_tasks.end());

	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk.getState() == ChunkState::VOXELS_GENERATED && m_tasks.size() < m_maxThreads)
		{
			notifyChunkNeighbours(pos);
			m_tasks.emplace_back(std::move(std::async(&Chunk::constructMesh, std::ref(chunk))));
		}
	}
}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader)
{
	for (const auto& [pos, chunk] : m_chunks)
	{
		if (chunk.getState() == ChunkState::READY && chunk.getState() != ChunkState::EMPTY)
		{
			chunk.draw(renderer, shader);
		}
	}
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
