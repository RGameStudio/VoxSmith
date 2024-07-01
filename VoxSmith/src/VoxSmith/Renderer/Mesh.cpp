#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "VoxSmith/Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

const glm::vec3 g_dirs[2][3] = {
	{
		{-1, 0, 0},
	{0, -1, 0},
	{0, 0, -1}
	},
	{
		{1, 0, 0},
	{0, 1, 0},
	{0, 0, 1},
	},
};

void Mesh::bakeStupid(const std::vector<Voxel>& voxels, const glm::vec3& cSize)
{
	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t z = 0; z < cSize.z; z++)
		{
			for (int32_t x = 0; x < cSize.x; x++)
			{
				const int32_t id = cSize.x * (y * cSize.z + z) + x;
				if (voxels[id].type == VoxelType::Empty)
				{
					continue;
				}

				const glm::vec3 xv = { x, y, z };
				for (int32_t iAxis = 0; iAxis < 3; iAxis++)
				{
					glm::vec3 voxelPos = { x, y, z };
					glm::vec3 u = glm::vec3(0.0f);
					glm::vec3 v = glm::vec3(0.0f);

					u[(iAxis + 1) % 3] = 1;
					v[(iAxis + 2) % 3] = 1;

					for (int32_t iSide = 0; iSide < 2; iSide++)
					{
						voxelPos[iAxis] = xv[iAxis] + iSide;
						addQuadFace(voxelPos, u, v);
					}
				}
			}
		}
	}
}

void Mesh::bakeCulled(const std::vector<Voxel>& voxels, const glm::vec3& cSize)
{
	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t z = 0; z < cSize.z; z++)
		{
			for (int32_t x = 0; x < cSize.x; x++)
			{
				const int32_t id = cSize.x * (y * cSize.z + z) + x;

				const glm::vec3 surroundingIDs[2] = {
					{
						id - 1,
						id - cSize.x * cSize.z,
						id - cSize.x
					},
					{
						id + 1,
						id + cSize.x * cSize.z,
						id + cSize.x
					}
				};

				if (voxels[id].type == VoxelType::Empty)
				{
					continue;
				}

				glm::vec3 voxelPos = { x, y, z };
				for (int32_t iAxis = 0; iAxis < 3; iAxis++)
				{					
					for (int32_t iSide = 0; iSide < 2; iSide++)
					{
						glm::vec3 u = glm::vec3(0.0f);
						glm::vec3 v = glm::vec3(0.0f);

						if (iSide > 0)
						{
							u[(iAxis + 1) % 3] = 1;
							v[(iAxis + 2) % 3] = 1;
						}
						else
						{
							u[(iAxis + 2) % 3] = 1;
							v[(iAxis + 1) % 3] = 1;
						}

						const int32_t neighbourID = surroundingIDs[iSide][iAxis];
						const glm::vec3 neighbourPos = voxelPos + g_dirs[iSide][iAxis];
						if (neighbourPos[iAxis] < 0 || neighbourPos[iAxis] >= cSize[iAxis] ||
							voxels.at(neighbourID).type == VoxelType::Empty)
						{
							voxelPos[iAxis] += iSide;
							addQuadFace(voxelPos, u, v);
						}
					}
				}
			}
		}
	}
}

void Mesh::addQuadFace(const glm::vec3& pos, const glm::vec3& u, const glm::vec3& v)
{
	m_vertices.push_back({ pos });
	m_vertices.push_back({ pos + u });
	m_vertices.push_back({ pos + v });

	m_vertices.push_back({ pos + v });
	m_vertices.push_back({ pos + u });
	m_vertices.push_back({ pos + u + v });
}

void Mesh::draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const
{
	if (renderer == nullptr)
	{
		return;
	}

	renderer->draw(m_buffer, shader, m_vertices.size());
}

void Mesh::loadToBuffer()
{
	m_buffer.init(m_vertices);
}
