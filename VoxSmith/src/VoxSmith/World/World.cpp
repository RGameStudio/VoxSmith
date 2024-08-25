#include <chrono>
#include <glm/glm.hpp>

#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"

#include "World.hpp"

using namespace VoxSmith;

constexpr float g_cSize = 32;
constexpr int32_t g_maxThreads = 3;
int32_t g_threadCounter = 0;


std::vector<std::future<glm::vec3>> m_tasks;

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
{
	m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_baseNoiseGen.SetFractalOctaves(16);
	m_baseNoiseGen.SetFrequency(0.0015f);
	m_baseNoiseGen.SetFractalLacunarity(2.0f);

	m_mountainNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_mountainNoiseGen.SetFractalType(FastNoiseLite::FractalType_Ridged);
	m_mountainNoiseGen.SetFractalOctaves(8);
	m_mountainNoiseGen.SetFrequency(0.0005f);
	m_mountainNoiseGen.SetFractalLacunarity(2.0f);

	for (int32_t y = minBoundary.y; y < maxBoundary.y; y += g_cSize)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z += g_cSize)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x += g_cSize)
			{
				auto pos = glm::vec3(x, y, z);
				m_meshes.push_back(std::make_shared<Mesh>());

				m_chunks[pos] = Chunk(pos, m_baseNoiseGen, m_mountainNoiseGen);				
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
					m_chunks[pos].loadVerticesToBuffer();
					g_threadCounter--;
					return true;
				}
				return false;
			}
		),
		m_tasks.end());

	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk.getState() == ChunkState::VOXESLS_GENERATED && g_threadCounter < g_maxThreads)
		{
			notifyChunkNeighbours(pos);
			m_tasks.push_back(std::async(&Chunk::constructMesh, std::ref(chunk)));
			g_threadCounter++;
		}
	}
}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader)
{
	for (auto& [pos, chunk] : m_chunks)
	{
		if (chunk.getState() == ChunkState::READY)
		{
			chunk.draw(renderer, shader);
		}
	}
}

void World::notifyChunkNeighbours(const glm::vec3& pos)
{
	auto checkAndAddNeighbour = [this, pos](const glm::vec3& neighbourPos, Direction dir) {
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
