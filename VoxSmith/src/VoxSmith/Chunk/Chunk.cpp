#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Renderer/Mesh.hpp"

#include "Voxel.hpp"

#include "Chunk.hpp"

using namespace VoxSmith;

constexpr glm::ivec3 g_dims = glm::ivec3(2);
constexpr uint32_t g_voxelsPerChunk = g_dims.x * g_dims.y * g_dims.z;

Chunk::Chunk(const glm::vec3& pos)
	: m_pos(pos)
{
	m_voxels.reserve(g_voxelsPerChunk);
	for (uint32_t y = 0; y < g_dims.y; y++)
	{
		for (uint32_t z = 0; z < g_dims.z; z++)
		{
			for (uint32_t x = 0; x < g_dims.x; x++)
			{
				m_voxels.emplace_back(VoxelType::Opaque);
			}
		}
	}
}

void Chunk::draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const
{
	if (m_mesh == nullptr || renderer == nullptr)
	{
		return;
	}

	m_mesh->draw(renderer, shader);
}

void Chunk::setMesh(const std::shared_ptr<Mesh>& mesh)
{
	m_mesh = mesh;
}

void Chunk::constructMesh()
{
	if (m_mesh == nullptr)
	{
		return;
	}

	m_mesh->bakeGreedy(m_voxels, g_dims.x);
	m_mesh->loadToBuffer();
}
