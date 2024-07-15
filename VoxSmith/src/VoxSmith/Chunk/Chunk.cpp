#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Renderer/Mesh.hpp"

#include "Chunk.hpp"

using namespace VoxSmith;

constexpr float g_sAxis = 3;
constexpr uint32_t g_voxelsPerChunk = g_sAxis * g_sAxis * g_sAxis;

Chunk::Chunk(const glm::vec3& pos)
	: m_pos(pos)
{
	m_voxels.reserve(g_voxelsPerChunk);
	for (uint32_t y = 0; y < g_sAxis; y++)
	{
		for (uint32_t z = 0; z < g_sAxis; z++)
		{
			for (uint32_t x = 0; x < g_sAxis; x++)
			{
				auto type = rand() % 2 == 0 ? VoxelType::Opaque : VoxelType::Empty;
				m_voxels.emplace_back(type);
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

	m_mesh->bakeGreedy(m_voxels, g_sAxis);
	m_mesh->loadToBuffer();
}
