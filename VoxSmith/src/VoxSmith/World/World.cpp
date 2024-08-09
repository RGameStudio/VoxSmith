#include <glm/glm.hpp>

#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Chunk/Chunk.hpp"
#include "VoxSmith/Shader/Shader.hpp"

#include "world.hpp"

using namespace VoxSmith;

constexpr float cSize = 32;

World::World(const glm::vec3 minBoundary, const glm::vec3 maxBoundary)
{
	for (int32_t y = minBoundary.y; y < maxBoundary.y; y += cSize)
	{
		for (int32_t z = minBoundary.z; z < maxBoundary.z; z += cSize)
		{
			for (int32_t x = minBoundary.x; x < maxBoundary.x; x += cSize)
			{
				auto pos = glm::vec3(x, y, z);
				m_chunks[pos] = Chunk(pos);
				sepcifyChunkNeighbours(pos);
			}
		}
	}
}

World::~World()
{

}

void World::update()
{

}

void World::draw(std::shared_ptr<Renderer>& renderer, const Shader& shader)
{
	for each (auto& pair in m_chunks)
	{
		pair.second.draw(renderer, shader);
	}
}

void World::sepcifyChunkNeighbours(const glm::vec3& pos)
{
	auto checkAndAddNeighbour = [this, pos](const glm::vec3& neighbourPos, Direction dir) {
		if (m_chunks.find(neighbourPos) != m_chunks.end())
		{
			m_chunks[pos].addNeighbour(dir, &m_chunks[neighbourPos]);
		}
	};

	checkAndAddNeighbour({ pos.x - cSize, pos.y, pos.z }, LEFT);
	checkAndAddNeighbour({ pos.x + cSize, pos.y, pos.z }, RIGHT);
	checkAndAddNeighbour({ pos.x, pos.y - cSize, pos.z }, BOTTOM);
	checkAndAddNeighbour({ pos.x, pos.y + cSize, pos.z }, TOP);
	checkAndAddNeighbour({ pos.x, pos.y, pos.z - cSize }, BACK);
	checkAndAddNeighbour({ pos.x, pos.y, pos.z + cSize }, FRONT);
}
