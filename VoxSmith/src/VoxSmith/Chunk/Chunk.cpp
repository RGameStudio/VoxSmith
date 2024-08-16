#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Renderer/Mesh.hpp"

#include "Chunk.hpp"

using namespace VoxSmith;

constexpr float g_sAxis = 32;
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


Chunk::Chunk(const glm::vec3& pos, const FastNoiseLite& noiseGenerator)
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
				auto type = VoxelType::Empty;

				int32_t height = 30 + 20 * noiseGenerator.GetNoise(pos.x + (float)x, pos.z + (float)z);

				if (y + pos.y < height - 1)
				{
					type = VoxelType::Dirt;
				}
				else if (y + pos.y == height - 1)
				{
					type = VoxelType::Grass;
				}
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

						bool isBackFace = !static_cast<bool>(iSide);
						defineUV(u, v, { 1, 1 }, isBackFace, iAxis);

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
									addQuadFace(voxelPos, iSide, iAxis, u, v, s_voxelColors[m_voxels.at(id)]);
								}
							}
							else
							{
								addQuadFace(voxelPos, iSide, iAxis, u, v, s_voxelColors[m_voxels.at(id)]);
							}
						}
						else if (voxels.at(neighbourID) == VoxelType::Empty)
						{
							addQuadFace(voxelPos, iSide, iAxis, u, v, s_voxelColors[m_voxels.at(id)]);
						}
					}
				}
			}
		}
	}
}

void Chunk::bakeGreedy(const std::vector<Voxel>& voxels, const float cSize)
{
	std::vector<VoxelType> faceMask(cSize * cSize, VoxelType::Empty);

	for (bool backFace = true, frontFace = false; frontFace != backFace; backFace = frontFace && backFace, frontFace = !frontFace)
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
								pos[iAxis] = 0;
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

						faceMask.at(n++) = backFace
							? bCurrent == VoxelType::Empty && bCompare != VoxelType::Empty ? bCompare : VoxelType::Empty
							: bCurrent != VoxelType::Empty && bCompare == VoxelType::Empty ? bCurrent : VoxelType::Empty;
					}
				}

				x[iAxis]++;
				n = 0;

				for (int32_t j = 0; j < cSize; j++)
				{
					for (int32_t i = 0; i < cSize; i)
					{
						if (faceMask[n] != VoxelType::Empty)
						{
							int32_t width;
							for (width = 1; i + width < cSize && faceMask[n + width] == faceMask[n]; width++)
							{
							}

							bool done = false;
							int32_t height;
							for (height = 1; j + height < cSize; height++)
							{
								for (int32_t w = 0; w < width; w++)
								{
									if (faceMask[n + w + height * cSize] != faceMask[n])
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

							defineUV(du, dv, { width, height }, backFace, iAxis);

							addQuadFace(x, du, dv, s_voxelColors[faceMask.at(n)]);

							for (int32_t h = 0; h < height; h++)
							{
								for (int32_t w = 0; w < width; w++)
								{
									faceMask.at(n + w + h * cSize) = VoxelType::Empty;
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

void Chunk::defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const bool backFace, const int32_t iAxis) const
{
	if (backFace)
	{
		u[(iAxis + 2) % 3] = size.y;
		v[(iAxis + 1) % 3] = size.x;
	}
	else
	{
		u[(iAxis + 1) % 3] = size.x;
		v[(iAxis + 2) % 3] = size.y;
	}
}

void Chunk::addQuadFace(glm::vec3& pos, const int32_t iSide, const int32_t iAxis, const glm::vec3& u, const glm::vec3& v, const glm::vec3& color)
{
	pos[iAxis] += iSide;

	m_vertices.push_back({ pos, color });
	m_vertices.push_back({ pos + u, color });
	m_vertices.push_back({ pos + v, color });

	m_vertices.push_back({ pos + v, color });
	m_vertices.push_back({ pos + u, color });
	m_vertices.push_back({ pos + u + v, color });
}

void Chunk::addQuadFace(const glm::vec3& pos, const glm::vec3& u, const glm::vec3& v, const glm::vec3& color)
{
	m_vertices.push_back({ pos, color });
	m_vertices.push_back({ pos + u, color });
	m_vertices.push_back({ pos + v, color });

	m_vertices.push_back({ pos + v, color });
	m_vertices.push_back({ pos + u, color });
	m_vertices.push_back({ pos + u + v, color });
}

void Chunk::draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const
{
	if (renderer == nullptr)
	{
		LOG_CORE_ERROR("CHUNK::DRAW::Renderer is not specified!");
		return;
	}

	if (m_mesh == nullptr)
	{
		LOG_CORE_ERROR("CHUNK::DRAW::Mesh is not initialized!");
		return;
	}

	shader.setUniform3fv("u_chunkPos", m_pos);
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
		LOG_CORE_ERROR("CHUNK::MESH::CONSTRUCTION::Can't construct mesh!");
		return;
	}

	bakeGreedy(m_voxels, g_sAxis);
	m_mesh->loadToBuffer(m_vertices);
}
