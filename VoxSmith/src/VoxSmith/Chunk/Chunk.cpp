#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Renderer/Mesh.hpp"

#include "Chunk.hpp"

using namespace VoxSmith;

constexpr float g_sAxis = 3;
constexpr uint32_t g_voxelsPerChunk = g_sAxis * g_sAxis * g_sAxis;

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


Chunk::Chunk(const glm::vec3& pos)
	: m_pos(pos)
	, m_neighbours(6, nullptr)
{
	m_voxels.reserve(g_voxelsPerChunk);
	for (uint32_t y = 0; y < g_sAxis; y++)
	{
		for (uint32_t z = 0; z < g_sAxis; z++)
		{
			for (uint32_t x = 0; x < g_sAxis; x++)
			{
				auto type = VoxelType::Opaque;
				m_voxels.emplace_back(type);
			}
		}
	}
}

void Chunk::addNeighbour(Direction dir, Chunk* chunk)
{
	m_neighbours.at(static_cast<int32_t>(dir)) = chunk;
}

int32_t Chunk::getId(const glm::vec3& v, const float cSize)
{
	return cSize * (v.y * cSize + v.z) + v.x;
}

void Chunk::bakeCulled(const std::vector<Voxel>& voxels, const float cSize)
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

						defineUV(u, v, { 1, 1 }, static_cast<FaceType>(iSide), iAxis);

						const int32_t neighbourID = surroundingIDs[iSide][iAxis];
						const glm::vec3 neighbourPos = voxelPos + g_dirs[iSide][iAxis];

						if (neighbourPos[iAxis] < 0 || neighbourPos[iAxis] >= cSize)
						{
							if (m_neighbours[iSide * 3 + iAxis] != nullptr)
							{
								glm::vec3 neighbourVoxelPos = glm::vec3(x, y, z);
								neighbourVoxelPos[iAxis] = neighbourPos[iAxis] < 0 ? cSize - 1 : 0;

								const auto& neighbourVoxels = m_neighbours[iSide * 3 + iAxis]->m_voxels;
								if (neighbourVoxels[getId(neighbourVoxelPos, cSize)] == VoxelType::Empty)
								{
									addQuadFace(voxelPos, iSide, iAxis, u, v);
								}
							}
							else
							{
								addQuadFace(voxelPos, iSide, iAxis, u, v);
							}
						}
						else if (voxels.at(neighbourID) == VoxelType::Empty)
						{
							addQuadFace(voxelPos, iSide, iAxis, u, v);
						}
					}
				}
			}
		}
	}
}

void Chunk::bakeGreedy(const std::vector<Voxel>& voxels, const float cSize)
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

			for (x[iAxis] = -1; x[iAxis] < cSize;)
			{
				int32_t n = 0;
				for (x[v] = 0; x[v] < cSize; x[v]++)
				{
					for (x[u] = 0; x[u] < cSize; x[u]++)
					{
#define CHECK_NEIGHBOURS true
#ifdef CHECK_NEIGHBOURS
						VoxelType bCurrent = VoxelType::Empty;
						VoxelType bCompare = VoxelType::Empty;

						if (0 <= x[iAxis])
						{
							bCurrent = voxels.at(getId(x, cSize));
						}
						else
						{
							if (m_neighbours[iAxis])
							{
								glm::vec3 pos = x;
								pos[iAxis] = cSize - 1;
								bCurrent = m_neighbours[iAxis]->m_voxels.at(getId(pos, cSize));
							}
							else
							{
								bCurrent = VoxelType::Empty;
							}
						}

						if (x[iAxis] < cSize - 1)
						{
							bCompare = voxels.at(getId(x + q, cSize));
						}
						else
						{
							if (m_neighbours[3 + iAxis])
							{
								glm::vec3 pos = x;
								pos[iAxis] = cSize - 1;
								bCompare = m_neighbours[3 + iAxis]->m_voxels.at(getId(pos, cSize));
							}
							else
							{
								bCompare = VoxelType::Empty;
							}
						}
#else
						auto bCurrent = 0 <= x[iAxis] ? voxels.at(getId(x, cSize)) : VoxelType::Empty;
						auto bCompare = x[iAxis] < cSize - 1 ? voxels.at(getId(x + q, cSize)) : VoxelType::Empty;
#endif

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

void Chunk::defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const FaceType face, const int32_t iAxis) const
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

void Chunk::addQuadFace(glm::vec3& pos, const int32_t iSide, const int32_t iAxis, const glm::vec3& u, const glm::vec3& v)
{
	pos[iAxis] += iSide;

	m_vertices.push_back({ pos });
	m_vertices.push_back({ pos + u });
	m_vertices.push_back({ pos + v });

	m_vertices.push_back({ pos + v });
	m_vertices.push_back({ pos + u });
	m_vertices.push_back({ pos + u + v });
}

void Chunk::addQuadFace(const glm::vec3& pos, const glm::vec3& u, const glm::vec3& v)
{
	m_vertices.push_back({ pos });
	m_vertices.push_back({ pos + u });
	m_vertices.push_back({ pos + v });

	m_vertices.push_back({ pos + v });
	m_vertices.push_back({ pos + u });
	m_vertices.push_back({ pos + u + v });
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

	bakeGreedy(m_voxels, g_sAxis);
	m_mesh->loadToBuffer(m_vertices);
}
