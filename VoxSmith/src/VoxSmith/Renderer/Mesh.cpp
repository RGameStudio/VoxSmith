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
				if (voxels.at(id) == VoxelType::Empty)
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

				if (voxels.at(id) == VoxelType::Empty)
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

						defineUV(u, v, {1, 1}, static_cast<FaceType>(iSide), iAxis);

						const int32_t neighbourID = surroundingIDs[iSide][iAxis];
						const glm::vec3 neighbourPos = voxelPos + g_dirs[iSide][iAxis];
						if (neighbourPos[iAxis] < 0 || neighbourPos[iAxis] >= cSize ||
							voxels.at(neighbourID) == VoxelType::Empty)
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
	std::vector<FaceType> mask(cSize * cSize, None);

	for (bool backFace = true, b = false; b != backFace; backFace = b && backFace, b = !b)
	{
		for (int32_t iAxis = 0; iAxis < 3; iAxis++)
		{
			int32_t u = (iAxis + 1) % 3;
			int32_t v = (iAxis + 2) % 3;

			glm::vec3 x = glm::vec3(0.0f);
			glm::vec3 q = glm::vec3(0.0f);

			q[iAxis] = 1;

			if (backFace == false)
			{
				int a = 5;
			}

			for (x[iAxis] = -1; x[iAxis] < cSize;)
			{
				int32_t n = 0;
				for (x[v] = 0; x[v] < cSize; x[v]++)
				{
					for (x[u] = 0; x[u] < cSize; x[u]++)
					{
						auto bCurrent = 0 <= x[iAxis] ? voxels.at(getId(x, cSize)) : VoxelType::Empty;
						auto bCompare = x[iAxis] < cSize - 1 ? voxels.at(getId(x + q, cSize)) : VoxelType::Empty;

						mask.at(n++) = backFace 
							? bCurrent == VoxelType::Empty && bCompare == VoxelType::Opaque ? BackFace : None 
							: bCurrent == VoxelType::Opaque && bCompare == VoxelType::Empty ? FrontFace : None;
					}
				}

				x[iAxis]++;
				n = 0;

				for (int32_t j = 0; j < cSize; j++)
				{
					for (int32_t i = 0; i < cSize; i)
					{
						if (mask[n] != None)
						{
							int32_t width;
							for (width = 1; i + width < cSize && mask[n + width] != None; width++)
							{
							}

							bool done = false;
							int32_t height;
							for (height = 1; j + height < cSize; height++)
							{
								for (int32_t w = 0; w < width; w++)
								{
									if (mask[n + w + height * cSize] == None)
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

							defineUV(du, dv, { width, height }, mask[n], iAxis);

							addQuadFace(x, du, dv);

							for (int32_t h = 0; h < height; h++)
							{
								for (int32_t w = 0; w < width; w++)
								{
									mask.at(n + w + h * cSize) = None;
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
}
#endif

void Mesh::defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const FaceType face, const int32_t iAxis) const
{
	if (face == FrontFace)
	{
		u[(iAxis + 1) % 3] = size.x;
		v[(iAxis + 2) % 3] = size.y;
	}
	else if (face == BackFace)
	{
		u[(iAxis + 2) % 3] = size.y;
		v[(iAxis + 1) % 3] = size.x;
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
