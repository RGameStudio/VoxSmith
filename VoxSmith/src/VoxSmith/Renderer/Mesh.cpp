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

				if (voxels[id].type == VoxelType::Empty)
				{
					continue;
				}

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

				for (int32_t iAxis = 0; iAxis < 3; iAxis++)
				{
					glm::vec3 voxelPos = { x, y, z };
					for (int32_t iSide = 0; iSide < 2; iSide++)
					{
						glm::vec3 u = glm::vec3(0.0f);
						glm::vec3 v = glm::vec3(0.0f);

						defineUV(u, v, iSide, iAxis);

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

int32_t Mesh::getId(const glm::vec3& v, const glm::vec3& cSize)
{
	return cSize.x * (v.y * cSize.z + v.z) + v.x;
}

void Mesh::bakeGreedy(const std::vector<Voxel>& voxels, const glm::vec3& cSize)
{
	for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b)
	{
		for (int32_t iDim = 0; iDim < 3; iDim++)
		{
			int32_t u = (iDim + 1) % 3;
			int32_t v = (iDim + 2) % 3;

			std::vector<bool> mask;
			mask.reserve(cSize[u] * cSize[v]);

			glm::vec3 x = { 0.0f, 0.0f, 0.0f };
			glm::vec3 q = { 0.0f, 0.0f, 0.0f };
			q[iDim] = 1.0f;

			for (x[iDim] = -1; x[iDim] < cSize.x; )
			{
				for (x[v] = 0; x[v] < cSize.y; x[v]++)
				{
					for (x[u] = 0; x[u] < cSize.x; x[u]++)
					{
						mask.push_back(
							(0 <= x[iDim] ? voxels.at(getId(x, cSize)).type : VoxelType::Empty) !=
							(x[iDim] < cSize[iDim] - 1 ? voxels.at(getId(x + q, cSize)).type : VoxelType::Empty)
						);
					}
				}
				
				x[iDim]++;
				int32_t n = 0;

				for (int32_t j = 0; j < cSize[v]; j++)
				{
					for (int32_t i = 0; i < cSize[u];)
					{
						if (mask[n])
						{
							// Compute width
							int32_t w;
							for (w = 1; mask[n + w] && i + w < cSize[u]; w++)
							{

							}

							// Compute height
							bool done = false;
							int32_t h;
							for (h = 1; j + h < cSize[v]; h++)
							{
								for (int32_t k = 0; k < w; k++)
								{
									if (!mask[n + k + h * cSize[u]])
									{
										done = true;
										break;
									}
								}
								if (done)
								{
									break;
								}
							}

							x[u] = i;
							x[v] = j;

							glm::vec3 du = glm::vec3(0.0f);
							glm::vec3 dv = glm::vec3(0.0f);
							du[u] = w;
							dv[v] = h;

							addQuadFace(x, du, dv);

							for (int32_t l = 0; l < h; l++)
							{
								for (int32_t k = 0; k < w; k++)
								{
									mask[n + k + l * cSize[u]] = false;
								}
							}

							i += w;
							n += w;
						}
						else
						{
							i++;
							n++;
						}
					}
				}
			}
		}
	}
}

void Mesh::defineUV(glm::vec3& u, glm::vec3& v, const int32_t iSide, const int32_t iAxis) const
{
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
