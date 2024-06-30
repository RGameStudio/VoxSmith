#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "VoxSmith/Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

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
				for (int32_t iDir = 0; iDir < 3; iDir++)
				{
					glm::vec3 voxelPos = { x, y, z };
					glm::vec3 u = glm::vec3(0.0f);
					glm::vec3 v = glm::vec3(0.0f);

					u[(iDir + 1) % 3] = 1;
					v[(iDir + 2) % 3] = 1;

					for (int32_t iSide = 0; iSide < 2; iSide++)
					{
						voxelPos[iDir] = xv[iDir] + iSide;
						addQuadFace(voxelPos, u, v);
					}
				}
			}
		}
	}
}

void Mesh::bakeCulled(const std::vector<Voxel>& voxels, const glm::vec3& cSize)
{
	const glm::vec3 dirs[3] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};

	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t z = 0; z < cSize.z; z++)
		{
			for (int32_t x = 0; x < cSize.x; x++)
			{
				const int32_t id = cSize.x * (y * cSize.z + z) + x;

				const glm::vec3 iDirs = {
					id + 1,
					id + cSize.x * cSize.z,
					id + cSize.x
				};

				for (int32_t iDir = 0; iDir < 3; iDir++)
				{
					glm::vec3 voxelPos = { x, y, z };
					glm::vec3 u = glm::vec3(0.0f);
					glm::vec3 v = glm::vec3(0.0f);

					u[(iDir + 1) % 3] = 1;
					v[(iDir + 2) % 3] = 1;

					if (voxels.at(id).type != VoxelType::Empty)
					{
						if (iDirs[iDir] < voxels.size() && voxels.at(iDirs[iDir]).type == VoxelType::Empty)
						{
							voxelPos += dirs[iDir];
							addQuadFace(voxelPos, u, v);
						}
					}
					else
					{
						if (iDirs[iDir] < voxels.size() && voxels.at(iDirs[iDir]).type == VoxelType::Opaque)
						{
							voxelPos += dirs[iDir];
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
