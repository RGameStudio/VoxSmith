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

void Mesh::bakeStupid(const std::vector<Voxel>& voxels, const float cSize)
{
	for (int32_t y = 0; y < cSize; y++)
	{
		for (int32_t z = 0; z < cSize; z++)
		{
			for (int32_t x = 0; x < cSize; x++)
			{
				const int32_t id = cSize * (y * cSize + z) + x;
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

void Mesh::bakeCulled(const std::vector<Voxel>& voxels, const float cSize)
{
	for (int32_t y = 0; y < cSize; y++)
	{
		for (int32_t z = 0; z < cSize; z++)
		{
			for (int32_t x = 0; x < cSize; x++)
			{
				const int32_t id = cSize * (y * cSize + z) + x;

				if (voxels[id].type == VoxelType::Empty)
				{
					continue;
				}

				const glm::vec3 surroundingIDs[2] = {
					{
						id - 1,
						id - cSize * cSize,
						id - cSize
					},
					{
						id + 1,
						id + cSize * cSize,
						id + cSize
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
						if (neighbourPos[iAxis] < 0 || neighbourPos[iAxis] >= cSize ||
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

int32_t Mesh::getId(const glm::vec3& v, const float cSize)
{
	return cSize * (v.y * cSize + v.z) + v.x;
}

bool checkRow()
{
	return false;
}

#define GREEDY_2D 0
#ifndef GREEDY_2D
void Mesh::bakeGreedy(const std::vector<Voxel>& voxels, const float cSize)
	std::vector<bool> mask;
	mask.reserve(cSize.x * cSize.y);

	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t x = 0; x < cSize.x; x++)
		{
			if (voxels.at(getId({ x, y, 0 }, cSize)).type == VoxelType::Opaque)
			{
				mask.push_back(true);
			}
		}
	}

	int32_t n = 0;
	for (int32_t y = 0; y < cSize.y; y++)
	{
		for (int32_t x = 0; x < cSize.x; x)
		{
			if (mask.at(n))
			{
				int32_t width;
				for (width = 1;
					x + width < cSize.x && voxels.at(getId({ x + width, y, 0 }, cSize)).type != VoxelType::Empty;
					width++)
				{
				}

				bool done = false;
				int32_t height;
				for (height = 1; y + height < cSize.y; height++)
				{
					for (int32_t w = 0; w < width; w++)
					{
						int32_t id = n + w + height * cSize.x;
						if (!mask.at(id))
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

				addQuadFace({ x, y, 0 }, { width, 0, 0 }, { 0, height, 0 });

				for (int32_t h = 0; h < height; h++)
				{
					for (int32_t w = 0; w < width; w++)
					{
						auto maskId = n + w + h * cSize.x;
						mask.at(maskId) = false;
					}
				}

				x += width;
				n += width;
			}
			else
			{
				x++;
				n++;
			}
		}
	}
#else
void Mesh::bakeGreedy(const std::vector<Voxel>& voxels, const float cSize)
{
	for (int32_t iDir = 0; iDir < 3; iDir++)
	{
		int32_t u = (iDir + 1) % 3;
		int32_t v = (iDir + 2) % 3;

		glm::vec3 x = glm::vec3(0.0f);
		glm::vec3 q = glm::vec3(0.0f);

		std::vector<bool> mask;
		mask.reserve(cSize * cSize);

		q[iDir] = 1;

		for (x[iDir] = -1; x[iDir] < cSize;)
		{
			int32_t n = 0;
			for (x[v] = 0; x[v] < cSize; x[v]++)
			{
				for (x[u] = 0; x[u] < cSize; x[u]++)
				{
					auto bCurrent = 0 <= x[iDir] ? voxels.at(getId(x, cSize)).type : VoxelType::Empty;
					auto bCompare = x[iDir] < cSize - 1 ? voxels.at(getId(x + q, cSize)).type : VoxelType::Empty;

					mask.push_back(bCurrent != bCompare);
					n++;
				}
			}

			x[iDir]++;
			n = 0;

			for (int32_t j = 0; j < cSize; j++)
			{
				for (int32_t i = 0; i < cSize; i)
				{
					if (mask[n])
					{
						int32_t width;
						for (width = 1; i + width < cSize && mask[n + width]; width++)
						{
						}

						bool done = false;
						int32_t height;
						for (height = 1; j + height < cSize; height++)
						{
							for (int32_t w = 0; w < width; w++)
							{
								if (!mask[n + w + height * cSize])
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

						glm::vec3 du = glm::vec3(0.0f);
						glm::vec3 dv = glm::vec3(0.0f);
						
						x[u] = i;
						x[v] = j;

						du[u] = width;
						dv[v] = height;

						addQuadFace(x, du, dv);

						for (int32_t h = 0; h < height; h++)
						{
							for (int32_t w = 0; w < width; w++)
							{
								mask[n + w + h * cSize] = false;
							}
						}

						n += width;
						i += width;
					}
					else
					{
						n++;
						i++;
					}
				}
			}
		}
	}
}
#endif

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
