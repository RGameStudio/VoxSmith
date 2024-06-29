#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "VoxSmith/Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

void Mesh::bakeStupid(const std::vector<Voxel>& voxels, const glm::vec3& pos, const glm::vec3& cSize)
{
	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t z = 0; z < cSize.z; z++)
		{
			for (int32_t x = 0; x < cSize.x; x++)
			{
				int32_t id = cSize.x * (y * cSize.z + z) + x;
				if (voxels[id].type == VoxelType::Empty)
				{
					continue;
				}

				glm::vec3 xv = { x, y, z };
				for (int32_t iDim = 0; iDim < 3; iDim++)
				{
					glm::vec3 voxelPos = { x, y, z };
					glm::vec3 u = { 0, 0, 0 };
					glm::vec3 v = { 0, 0, 0 };

					u[(iDim + 1) % 3] = 1;
					v[(iDim + 2) % 3] = 1;

					for (int32_t iSide = 0; iSide < 2; iSide++)
					{
						voxelPos[iDim] = xv[iDim] + iSide;
						m_vertices.push_back({ voxelPos });
						m_vertices.push_back({ voxelPos + u });
						m_vertices.push_back({ voxelPos + v });

						m_vertices.push_back({ voxelPos + v });
						m_vertices.push_back({ voxelPos + u });
						m_vertices.push_back({ voxelPos + u + v });
					}
				}
			}
		}
	}	
}

void Mesh::draw(const Renderer& renderer, const Shader& shader) const
{
	renderer.draw(m_buffer, shader, m_vertices.size());
}

void Mesh::loadToBuffer()
{
	m_buffer.init(m_vertices);
}
