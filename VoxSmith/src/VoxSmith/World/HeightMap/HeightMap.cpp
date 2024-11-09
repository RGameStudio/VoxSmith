#include "VoxSmith/Logger/Log.hpp"

#include "HeightMap.hpp"

using namespace VoxSmith;

void HeightMap::update()
{

}

ChunkMap HeightMap::getChunkHeight(const glm::ivec2& pos)
{
	if (m_map.find(pos) == m_map.end())
	{
		LOG_CORE_ERROR("HEIGHTMAP::ERROR::Could not fight chunk map at {0} {1}!", pos.x, pos.y);
		return ChunkMap();
	}

	return m_map[pos];
}
