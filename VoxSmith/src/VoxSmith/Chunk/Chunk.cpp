#include "Voxel.hpp"

#include "Chunk.hpp"

using namespace VoxSmith;

constexpr glm::ivec3 g_dims = glm::ivec3(64);
constexpr uint32_t g_voxelsPerChunk = g_dims.x * g_dims.y * g_dims.z;

Chunk::Chunk(const glm::vec3& pos)
	: m_pos(pos)
{
	m_voxels.reserve(g_voxelsPerChunk);
	for (uint32_t z = 0; z < g_dims.z; z++)
	{
		for (uint32_t y = 0; y < g_dims.y; y++)
		{
			for (uint32_t x = 0; x < g_dims.x; x++)
			{
				Voxel voxel = { VoxelType::OPAQUE };
				m_voxels.emplace_back(std::move(voxel));
			}
		}
	}
}
