#include "VoxSmith/Camera/Camera.hpp"
#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Mesh.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/ResourceManager/ResourceManager.hpp"
#include "VoxSmith/ResourceManager/ResourcesLists.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/World/HeightMap/HeightMap.hpp"
#include "VoxSmith/World/World.hpp"
#include "VoxSmith/World/WorldManager.hpp"
#include "Voxel.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <winuser.h>

#include "Chunk.hpp"

using namespace VoxSmith;

constexpr uint32_t g_sAxis = 32;
constexpr uint32_t g_sAxisPadded = g_sAxis + 2;
constexpr uint32_t g_sChunkSizePadded =
    g_sAxisPadded * g_sAxisPadded * g_sAxisPadded;
constexpr uint32_t g_sGridPlaneSizePadded = g_sAxisPadded * g_sAxisPadded;
constexpr uint32_t g_sVoxelsPerChunk = g_sAxis * g_sAxis * g_sAxis;
constexpr uint32_t g_sGridPlaneSize = g_sAxis * g_sAxis;
constexpr uint32_t g_biomThreshold = 5;

const glm::vec3 g_dirs[2][3] = {
    {{-1, 0, 0}, {0, -1, 0}, {0, 0, -1}},
    {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
    },
};

constexpr uint32_t g_nFaces = 6;
constexpr uint32_t g_nNeighboursPerFace = 8;

const glm::vec3 g_aoStates[g_nFaces][g_nNeighboursPerFace] = {
    {
        // Left face
        {-1, -1, -1},
        {-1, -1, 0},
        {-1, -1, 1},
        {-1, 0, 1},
        {-1, 1, 1},
        {-1, 1, 0},
        {-1, 1, -1},
        {-1, 0, -1},
    },
    {
        // Bottom face
        {-1, -1, -1},
        {0, -1, -1},
        {1, -1, -1},
        {1, -1, 0},
        {1, -1, 1},
        {0, -1, 1},
        {-1, -1, 1},
        {-1, -1, 0},
    },
    {
        // Back face
        {-1, -1, -1},
        {-1, 0, -1},
        {-1, 1, -1},
        {0, 1, -1},
        {1, 1, -1},
        {1, 0, -1},
        {1, -1, -1},
        {0, -1, -1},
    },
    {
        // Right face
        {1, -1, -1},
        {1, 0, -1},
        {1, 1, -1},
        {1, 1, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, -1, 1},
        {1, -1, 0},
    },
    {
        // Top face
        {-1, 1, -1},
        {-1, 1, 0},
        {-1, 1, 1},
        {0, 1, 1},
        {1, 1, 1},
        {1, 1, 0},
        {1, 1, -1},
        {0, 1, -1},
    },
    {
        // Front face
        {-1, -1, 1},
        {0, -1, 1},
        {1, -1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 1},
        {-1, 1, 1},
        {-1, 0, 1},
    },
};

const glm::vec3 g_localPaddedNeighbourPos[26] = {
    {g_sAxisPadded - 2, 0, 0}, // to LEFT
    {0, g_sAxisPadded - 2, 0}, // to BOTTOM
    {0, 0, g_sAxisPadded - 2}, // to BACK
    {1, 0, 0},                 // to RIGHT
    {0, 1, 0},                 // to TOP
    {0, 0, 1},                 // to FRONT

    {g_sAxisPadded - 2, g_sAxisPadded - 2,
     g_sAxisPadded - 2},                       // to BOTTOM_LEFT_BACK
    {g_sAxisPadded - 2, g_sAxisPadded - 2, 1}, // to BOTTOM_LEFT_FRONT
    {1, g_sAxisPadded - 2, g_sAxisPadded - 2}, // to BOTTOM_RIGHT_BACK
    {1, g_sAxisPadded - 2, 1},                 // to BOTTOM_RIGHT_FRONT

    {g_sAxisPadded - 2, 1, g_sAxisPadded - 2}, // to TOP_LEFT_BACK
    {g_sAxisPadded - 2, 1, 1},                 // to TOP_LEFT_FRONT
    {1, 1, g_sAxisPadded - 2},                 // to TOP_RIGHT_BACK
    {1, 1, 1},                                 // to TOP_RIGHT_FRONT

    {1, 0, 1},                                 // to FRONT_RIGHT
    {g_sAxisPadded - 2, 0, 1},                 // to FRONT_LEFT
    {1, 0, g_sAxisPadded - 2},                 // to BACK_RIGHT
    {g_sAxisPadded - 2, 0, g_sAxisPadded - 2}, // to BACK_LEFT

    {0, 1, 1},                                 // to TOP_FRONT
    {0, 1, g_sAxisPadded - 2},                 // to TOP_BACK
    {1, g_sAxisPadded - 2, 0},                 // to BOTTOM_FRONT
    {0, g_sAxisPadded - 2, g_sAxisPadded - 2}, // to BOTTOM_BACK

    {1, 1, 0},                                 // to TOP_RIGHT
    {g_sAxisPadded - 2, 1, 0},                 // to TOP_LEFT
    {1, g_sAxisPadded - 2, 0},                 // to BOTTOM_RIGHT
    {g_sAxisPadded - 2, g_sAxisPadded - 2, 0}, // to BOTTOM_LEFT
};

Chunk::Chunk(const glm::vec3 &pos)
    : m_pos(pos), m_state(ChunkState::EMPTY),
      m_voxels(g_sChunkSizePadded, Voxel::Empty),
      m_center(pos + glm::vec3(g_sAxis / 2)), m_neighbours(26, nullptr),
      m_mesh(std::make_shared<Mesh>()), m_opaqueMask(g_sGridPlaneSizePadded, 0)
{
    std::unique_lock<std::shared_mutex> uLock(m_mutex);
    m_state = ChunkState::EMPTY;
    uLock.unlock();
}

Chunk::~Chunk() { std::lock_guard<std::shared_mutex> lock(m_mutex); }

void Chunk::generateChunk(const ChunkMap &map)
{
    std::unique_lock<std::shared_mutex> uLock(m_mutex);
    m_state = ChunkState::FRESH;
    uLock.unlock();

    bool hasOnlyEmpty = true;
    for (uint32_t y = 1; y < g_sAxisPadded - 1; y++)
    {
        for (uint32_t z = 1; z < g_sAxisPadded - 1; z++)
        {
            for (uint32_t x = 1; x < g_sAxisPadded - 1; x++)
            {
                auto type = VoxelType::Empty;

                const int32_t height = map.data[(z - 1) * g_sAxis + (x - 1)];
                const auto biome = map.biomData[(z - 1) * g_sAxis + (x - 1)];

                if (y + m_pos.y < height - g_biomThreshold)
                {
                    type = VoxelType::Stone;
                }
                else if (y + m_pos.y < height)
                {
                    type = biome.mainVoxel;
                }
                else if (y + m_pos.y == height)
                {
                    type = biome.topVoxel;
                }

                if (type != VoxelType::Empty)
                {
                    hasOnlyEmpty = false;
                    m_opaqueMask.at(y * g_sAxisPadded + z) |= 1 << x;
                }
                m_voxels.at(getId({x, y, z}, g_sAxisPadded)) = type;
            }
        }
    }

    if (!hasOnlyEmpty)
    {
        uLock.lock();
        m_state = ChunkState::VOXELS_GENERATED;
    }
    else
    {
        uLock.lock();
        m_state = ChunkState::VOXELS_GENERATED_READY;
    }
}

Voxel Chunk::getVoxel(const glm::vec3 pLocal) const
{
    const int32_t id = getId(pLocal, g_sAxis);

    if (m_state == ChunkState::EMPTY)
    {
        return Voxel::Empty;
    }

    if (id < 0 || id >= g_sVoxelsPerChunk)
    {
        LOG_CORE_ERROR("CHUNK::GET_VOXEL::Could not get voxel.");
        return Voxel::Empty;
    }

    return m_voxels.at(id);
}

void Chunk::updateFace(Direction dir, const std::shared_ptr<Chunk> &neighbour)
{
    const int32_t iAxis = dir % 3;
    const int32_t iuAxis = (dir + 1) % 3;
    const int32_t ivAxis = (dir + 2) % 3;

    const int32_t iFaceAxis = dir > Direction::BACK ? g_sAxisPadded - 1 : 0;

    glm::vec3 neighbourPos = g_localPaddedNeighbourPos[dir];

    for (int32_t v = 1; v < g_sAxisPadded - 1; v++)
    {
        for (int32_t u = 1; u < g_sAxisPadded - 1; u++)
        {
            glm::vec3 localPos = glm::vec3(0.0f);
            localPos[iuAxis] = u;
            localPos[ivAxis] = v;
            localPos[iAxis] = iFaceAxis;

            neighbourPos[iuAxis] = u;
            neighbourPos[ivAxis] = v;

            const int32_t localId = getId(localPos, g_sAxisPadded);
            const int32_t neighbourId = getId(neighbourPos, g_sAxisPadded);

            m_voxels.at(localId) = neighbour->m_voxels.at(neighbourId);
        }
    }
}

void Chunk::updateEdge(Direction dir, const std::shared_ptr<Chunk> &neighbour)
{
}

void Chunk::udpateCorner(Direction dir, const std::shared_ptr<Chunk> &neighbour)
{
}

void Chunk::setState(ChunkState state)
{
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    m_state = state;
}

ChunkState Chunk::getState() const
{
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    return m_state;
}

Direction VoxSmith::getInverseDirection(Direction dir)
{
    return static_cast<Direction>((static_cast<int32_t>(dir) + 3) % 6);
}

void Chunk::updatePadding()
{
    for (int32_t iDir = Direction::LEFT; iDir < BOTTOM_LEFT; iDir++)
    {
        const auto &chunk = m_neighbours[iDir];
        auto dir = static_cast<Direction>(iDir);
        if (dir <= FRONT) // update padded voxels for faces
        {
            updateFace(dir, chunk);
        }
        else if (dir >= TOP_LEFT_BACK &&
                 dir <= BOTTOM_RIGHT_FRONT) // update corners
        {
        }
        else // update edges
        {
        }
    }
}

void Chunk::addNeighbour(Direction dir, const std::shared_ptr<Chunk> &chunk)
{
    m_neighbours.at(static_cast<int32_t>(dir)) = chunk;
}

bool Chunk::canBake() const
{
    for (auto &neighbour : m_neighbours)
    {
        if (neighbour == nullptr ||
            neighbour->getState() == ChunkState::FRESH ||
            neighbour->getState() == ChunkState::EMPTY)
        {
            return false;
        }
    }

    return true;
}

bool Chunk::inFrustum(const Frustum &frustum)
{
    return isOnwardPlane(frustum.bottomFace) &&
           isOnwardPlane(frustum.topFace) && isOnwardPlane(frustum.nearFace) &&
           isOnwardPlane(frustum.farFace) && isOnwardPlane(frustum.leftFace) &&
           isOnwardPlane(frustum.rightFace);
}

bool Chunk::isOnwardPlane(const Plane &plane) const
{
    const float extent = g_sAxis * 0.5f;
    const float r = extent * glm::abs(plane.normal.x) +
                    extent * glm::abs(plane.normal.y) +
                    extent * glm::abs(plane.normal.z);
    return glm::dot(plane.normal, m_center) - plane.distance + r >= 0;
}

bool Chunk::inBounds(const glm::ivec3 &min, const glm::ivec3 &max) const
{
    return min.x <= m_pos.x && min.y <= m_pos.y && min.z <= m_pos.z &&
           m_pos.x <= max.x && m_pos.y <= max.y && m_pos.z <= max.z;
}

void Chunk::bakeCulled(const std::vector<Voxel> &voxels, const float cSize)
{
    for (int32_t y = 1; y < g_sAxisPadded - 1; y++)
    {
        for (int32_t z = 1; z < g_sAxisPadded - 1; z++)
        {
            for (int32_t x = 1; x < g_sAxisPadded - 1; x++)
            {
                const int32_t id = getId({x, y, z}, g_sAxisPadded);

                if (voxels.at(id) == VoxelType::Empty)
                {
                    continue;
                }

                const int32_t left = getId({x - 1, y, z}, g_sAxisPadded);
                const int32_t right = getId({x + 1, y, z}, g_sAxisPadded);

                const int32_t bottom = getId({x, y - 1, z}, g_sAxisPadded);
                const int32_t top = getId({x, y + 1, z}, g_sAxisPadded);

                const int32_t back = getId({x, y, z - 1}, g_sAxisPadded);
                const int32_t front = getId({x, y, z + 1}, g_sAxisPadded);

                const glm::vec3 voxelPos = {x - 1, y - 1, z - 1};

                // TODO: Improve in the future with putting this in the loop;
                bool faceFree = m_voxels.at(left) == VoxelType::Empty;
                if (faceFree)
                {
                    const glm::vec3 facePos = {x - 1, y - 1, z - 1};
                    addQuadFace(voxelPos, facePos, {0, 0, 1}, {0, 1, 0},
                                s_textureFaces[m_voxels.at(id)][LEFT], LEFT,
                                false);
                }

                faceFree = m_voxels.at(right) == VoxelType::Empty;
                if (faceFree)
                {
                    const glm::vec3 facePos = {x, y - 1, z - 1};
                    addQuadFace(voxelPos, facePos, {0, 0, 1}, {0, 1, 0},
                                s_textureFaces[m_voxels.at(id)][RIGHT], RIGHT,
                                true);
                }

                faceFree = m_voxels.at(bottom) == VoxelType::Empty;
                if (faceFree)
                {
                    const glm::vec3 facePos = {x - 1, y - 1, z - 1};
                    addQuadFace(voxelPos, facePos, {0, 0, 1}, {1, 0, 0},
                                s_textureFaces[m_voxels.at(id)][BOTTOM], BOTTOM,
                                true);
                }

                faceFree = m_voxels.at(top) == VoxelType::Empty;
                if (faceFree)
                {
                    const glm::vec3 facePos = {x - 1, y, z - 1};
                    addQuadFace(voxelPos, facePos, {0, 0, 1}, {1, 0, 0},
                                s_textureFaces[m_voxels.at(id)][TOP], TOP,
                                false);
                }

                faceFree = m_voxels.at(back) == VoxelType::Empty;
                if (faceFree)
                {
                    const glm::vec3 facePos = {x - 1, y - 1, z - 1};
                    addQuadFace(voxelPos, facePos, {1, 0, 0}, {0, 1, 0},
                                s_textureFaces[m_voxels.at(id)][BACK], BACK,
                                true);
                }

                faceFree = m_voxels.at(front) == VoxelType::Empty;
                if (faceFree)
                {
                    const glm::vec3 facePos = {x - 1, y - 1, z};
                    addQuadFace(voxelPos, facePos, {1, 0, 0}, {0, 1, 0},
                                s_textureFaces[m_voxels.at(id)][FRONT], FRONT,
                                false);
                }
            }
        }
    }
}

void Chunk::bakeGreedy(const std::vector<Voxel> &voxels, const int32_t size)
{
    for (int32_t iFace = 0; iFace < 2; iFace++) // 0 - backface, 1 - frontface
    {
        for (int32_t iAxis = 0; iAxis < 3; iAxis++)
        {
            const int32_t iuAxis = (iAxis + 1) % 3;
            const int32_t ivAxis = (iAxis + 2) % 3;

            for (int32_t iPlane = 0; iPlane < size; iPlane++)
            {
                std::vector<Voxel> faceMask(size * size, Voxel::Empty);
                std::vector<bool> visitedMask(size * size, false);

                glm::vec3 pVoxelInPlane = glm::vec3(0.0f);
                glm::vec3 pVoxelToCompare = glm::vec3(0.0f);

                pVoxelInPlane[iAxis] = iPlane;
                pVoxelToCompare[iAxis] =
                    iFace == 0
                        ? iPlane - 1
                        : iPlane + 1; // If we are creating backfaces make sure,
                                      // to check the corresponding voxel with
                                      // the back plane -> iPlane - 1, and front
                                      // plane accordingly -> iPlane + 1

                int32_t iMask = 0;
                for (int32_t v = 0; v < size; v++)
                {
                    for (int32_t u = 0; u < size; u++)
                    {
                        pVoxelInPlane[iuAxis] = u;
                        pVoxelInPlane[ivAxis] = v;

                        pVoxelToCompare[iuAxis] = u;
                        pVoxelToCompare[ivAxis] = v;

                        const Voxel currVoxel =
                            m_voxels.at(getId(pVoxelInPlane, size));
                        Voxel compareVoxel = VoxelType::Empty;

                        if (pVoxelToCompare[iAxis] <
                            0) // In case the neighbour chunk for the backface
                               // is needed
                        {
                            const auto &chunk = m_neighbours[iAxis];
                            glm::vec3 localPos = pVoxelToCompare;
                            localPos[iAxis] = size - 1;
                            compareVoxel =
                                chunk->m_voxels.at(getId(localPos, size));
                        }
                        else if (pVoxelToCompare[iAxis] >= size)
                        {
                            const auto &chunk = m_neighbours[iAxis + 3];
                            glm::vec3 localPos = pVoxelToCompare;
                            localPos[iAxis] = 0;
                            compareVoxel =
                                chunk->m_voxels.at(getId(localPos, size));
                        }
                        else
                        {
                            compareVoxel =
                                m_voxels.at(getId(pVoxelToCompare, size));
                        }

                        faceMask.at(iMask++) =
                            currVoxel != Voxel::Empty &&
                                    compareVoxel == Voxel::Empty
                                ? currVoxel
                                : Voxel::Empty;
                    }
                }

                for (int32_t v = 0; v < size; v++)
                {
                    for (int32_t u = 0; u < size; u++)
                    {
                        pVoxelInPlane[iuAxis] = u;
                        pVoxelInPlane[ivAxis] = v;

                        iMask = size * v + u;
                        if (faceMask.at(iMask) == Voxel::Empty)
                        {
                            continue;
                        }

                        const Voxel currVoxel = faceMask.at(iMask);

                        int32_t width = 1;
                        glm::vec3 pAttachVoxel = pVoxelInPlane;
                        pAttachVoxel[iuAxis]++;
                        for (; width + u < size &&
                               faceMask.at(iMask + width) == currVoxel &&
                               compareVerticesAO(pVoxelInPlane, pAttachVoxel,
                                                 iAxis + iFace * 3);
                             pAttachVoxel[iuAxis]++, width++)
                        {
                            ;
                            ;
                        }

                        bool quadDone = false;

                        int32_t height = 1;
                        pAttachVoxel = pVoxelInPlane;
                        pAttachVoxel[ivAxis]++;
                        for (; v + height < size;
                             height++, pAttachVoxel[ivAxis]++)
                        {
                            pAttachVoxel[iuAxis] = pVoxelInPlane[iuAxis];
                            for (int32_t w = 0; w < width;
                                 w++, pAttachVoxel[iuAxis]++)
                            {
                                if (faceMask.at(iMask + height * size + w) !=
                                        currVoxel ||
                                    !compareVerticesAO(pVoxelInPlane,
                                                       pAttachVoxel,
                                                       iAxis + iFace * 3))
                                {
                                    quadDone = true;
                                    break;
                                }
                            }

                            if (quadDone)
                            {
                                break;
                            }
                        }

                        glm::vec3 du = glm::vec3(0.0);
                        glm::vec3 dv = glm::vec3(0.0);

                        du[iuAxis] = width;
                        dv[ivAxis] = height;

                        const int32_t texId =
                            iFace == 0 ? s_textureFaces[currVoxel][iAxis]
                                       : s_textureFaces[currVoxel][iAxis + 3];

                        glm::vec3 facePos = pVoxelInPlane;

                        bool backface = iFace == 0;

                        glm::ivec2 dims = {du[iuAxis], dv[ivAxis]};

                        if (iAxis == 0)
                        {
                            std::swap(du, dv);
                            std::swap(dims.x, dims.y);
                            backface = !backface;
                        }

                        if (iFace == 1)
                        {
                            facePos[iAxis] += 1.0f;
                        }

                        addQuadFace(pVoxelInPlane, facePos, du, dv, dims, texId,
                                    iAxis + iFace * 3, backface);

                        for (int32_t h = 0; h < height; h++)
                        {
                            for (int32_t w = 0; w < width; w++)
                            {
                                faceMask.at(iMask + h * size + w) =
                                    Voxel::Empty;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Chunk::bakeBinGreedy(const std::vector<Voxel> &voxels, const float size)
{
    std::vector<uint32_t> faceMasks(g_sGridPlaneSize * 6, 0);

    for (int32_t iHeight = 0; iHeight < size; iHeight++)
    {
        for (int32_t iWidth = 0; iWidth < size; iWidth++)
        {
            const uint32_t bits = m_opaqueMask.at(iHeight * size + iWidth);

            const int32_t iwPlane = iHeight * size + iWidth;
            const int32_t ihPlane = iWidth * size + iHeight;

            faceMasks.at(g_sGridPlaneSize * 0 + iwPlane) =
                bits & ~m_opaqueMask.at((iHeight + 1) * size + iWidth);
            faceMasks.at(g_sGridPlaneSize * 1 + iwPlane) =
                bits & ~m_opaqueMask.at((iHeight - 1) * size + iWidth);

            faceMasks.at(g_sGridPlaneSize * 2 + ihPlane) =
                bits & ~m_opaqueMask.at(iHeight * size + iWidth + 1);
            faceMasks.at(g_sGridPlaneSize * 3 + ihPlane) =
                bits & ~m_opaqueMask.at(iHeight * size + iWidth - 1);

            faceMasks.at(g_sGridPlaneSize * 4 + iwPlane) = bits & ~(bits >> 1);
            faceMasks.at(g_sGridPlaneSize * 5 + iwPlane) = bits & ~(bits << 1);
        }
    }
}

void Chunk::addQuadFace(const glm::vec3 &pos, const glm::vec3 &u,
                        const glm::vec3 &v, const glm::u8vec3 &color,
                        const int32_t id)
{
    m_vertices.emplace_back(pos, color, id);
    m_vertices.emplace_back(pos + u, color, id);
    m_vertices.emplace_back(pos + v, color, id);

    m_vertices.emplace_back(pos + v, color, id);
    m_vertices.emplace_back(pos + u, color, id);
    m_vertices.emplace_back(pos + u + v, color, id);
}

void Chunk::addQuadFace(const glm::vec3 &pos, const glm::vec3 &u,
                        const glm::vec3 &v, const int32_t texId,
                        bool isBackFace)
{
    if (isBackFace)
    {
        m_vertices.emplace_back(pos, texId, 0);
        m_vertices.emplace_back(pos + v, texId, 2);
        m_vertices.emplace_back(pos + u, texId, 1);

        m_vertices.emplace_back(pos + u, texId, 1);
        m_vertices.emplace_back(pos + v, texId, 2);
        m_vertices.emplace_back(pos + u + v, texId, 3);
    }
    else
    {
        m_vertices.emplace_back(pos, texId, 0);
        m_vertices.emplace_back(pos + u, texId, 1);
        m_vertices.emplace_back(pos + v, texId, 2);

        m_vertices.emplace_back(pos + v, texId, 2);
        m_vertices.emplace_back(pos + u, texId, 1);
        m_vertices.emplace_back(pos + u + v, texId, 3);
    }
}

void Chunk::addQuadFace(const glm::vec3 &pos, const glm::vec3 &u,
                        const glm::vec3 &v, const glm::ivec2 &dims,
                        const int32_t texId, bool isBackFace)
{
    if (isBackFace)
    {
        m_vertices.emplace_back(pos, texId, 0, dims);
        m_vertices.emplace_back(pos + v, texId, 2, dims);
        m_vertices.emplace_back(pos + u, texId, 1, dims);

        m_vertices.emplace_back(pos + u, texId, 1, dims);
        m_vertices.emplace_back(pos + v, texId, 2, dims);
        m_vertices.emplace_back(pos + u + v, texId, 3, dims);
    }
    else
    {
        m_vertices.emplace_back(pos, texId, 0, dims);
        m_vertices.emplace_back(pos + u, texId, 1, dims);
        m_vertices.emplace_back(pos + v, texId, 2, dims);

        m_vertices.emplace_back(pos + v, texId, 2, dims);
        m_vertices.emplace_back(pos + u, texId, 1, dims);
        m_vertices.emplace_back(pos + u + v, texId, 3, dims);
    }
}

uint32_t Chunk::vertexAO(int32_t side1, int32_t side2, int32_t corner) const
{
    if (side1 + side2 == 2)
    {
        return 0;
    }

    return 3 - (side1 + side2 + corner);
}

bool Chunk::compareVerticesAO(const glm::vec3 &pVoxel1,
                              const glm::vec3 &pVoxel2,
                              const int32_t faceId) const
{
    const auto &faceAODirs = g_aoStates[faceId];

    const int32_t p1v1 =
        checkVertexAO(pVoxel1, faceAODirs[0], faceAODirs[1], faceAODirs[7]);
    const int32_t p1v2 =
        checkVertexAO(pVoxel1, faceAODirs[2], faceAODirs[1], faceAODirs[3]);
    const int32_t p1v3 =
        checkVertexAO(pVoxel1, faceAODirs[6], faceAODirs[5], faceAODirs[7]);
    const int32_t p1v4 =
        checkVertexAO(pVoxel1, faceAODirs[4], faceAODirs[5], faceAODirs[3]);

    const int32_t p2v1 =
        checkVertexAO(pVoxel2, faceAODirs[0], faceAODirs[1], faceAODirs[7]);
    const int32_t p2v2 =
        checkVertexAO(pVoxel2, faceAODirs[2], faceAODirs[1], faceAODirs[3]);
    const int32_t p2v3 =
        checkVertexAO(pVoxel2, faceAODirs[6], faceAODirs[5], faceAODirs[7]);
    const int32_t p2v4 =
        checkVertexAO(pVoxel2, faceAODirs[4], faceAODirs[5], faceAODirs[3]);

    return p1v1 == p2v1 && p1v2 == p2v2 && p1v3 == p2v3 && p1v4 == p2v4;
}

uint32_t Chunk::checkVertexAO(const glm::vec3 &pos, const glm::vec3 &vCorner,
                              const glm::vec3 &vSide1,
                              const glm::vec3 &vSide2) const
{
    const glm::vec3 localPos = pos + glm::vec3(1, 1, 1);

    const glm::vec3 pCorner = localPos + vCorner;
    const glm::vec3 pSide1 = localPos + vSide1;
    const glm::vec3 pSide2 = localPos + vSide2;

    const int32_t cornerId = getId(pCorner, g_sAxisPadded);
    const int32_t side1Id = getId(pSide1, g_sAxisPadded);
    const int32_t side2Id = getId(pSide2, g_sAxisPadded);

    Voxel corner = m_voxels.at(cornerId);
    Voxel side1 = m_voxels.at(side1Id);
    Voxel side2 = m_voxels.at(side2Id);

    return vertexAO(side1 != VoxelType::Empty ? 1 : 0,
                    side2 != VoxelType::Empty ? 1 : 0,
                    corner != VoxelType::Empty ? 1 : 0);
}

void Chunk::addQuadFace(const glm::vec3 &pos, const glm::vec3 &facePos,
                        const glm::vec3 &u, const glm::vec3 &v,
                        const glm::ivec2 &dims, const int32_t texId,
                        const int32_t faceId, bool isBackFace)
{
    if (faceId >= g_nFaces)
    {
        LOG_CORE_ERROR("Error: faceId - {0} is wrong ", faceId);
        return;
    }

    const auto &faceAODirs = g_aoStates[faceId];

    if (isBackFace)
    {
        m_vertices.emplace_back(
            facePos, texId, 0, dims,
            checkVertexAO(pos, faceAODirs[0], faceAODirs[1], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + v, texId, 2, dims,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + u, texId, 1, dims,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));

        m_vertices.emplace_back(
            facePos + u, texId, 1, dims,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + v, texId, 2, dims,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + u + v, texId, 3, dims,
            checkVertexAO(pos, faceAODirs[4], faceAODirs[3], faceAODirs[5]));
    }
    else
    {
        m_vertices.emplace_back(
            facePos, texId, 0, dims,
            checkVertexAO(pos, faceAODirs[0], faceAODirs[1], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + u, texId, 1, dims,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + v, texId, 2, dims,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));

        m_vertices.emplace_back(
            facePos + v, texId, 2, dims,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + u, texId, 1, dims,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + u + v, texId, 3, dims,
            checkVertexAO(pos, faceAODirs[4], faceAODirs[3], faceAODirs[5]));
    }
}

void Chunk::addQuadFace(const glm::vec3 &pos, const glm::vec3 &facePos,
                        const glm::vec3 &u, const glm::vec3 &v,
                        const int32_t texId, const int32_t faceId,
                        bool isBackFace)
{
    if (faceId >= g_nFaces)
    {
        LOG_CORE_ERROR("Error: faceId - {0} is wrong ", faceId);
        return;
    }

    const auto &faceAODirs = g_aoStates[faceId];

    if (isBackFace)
    {
        m_vertices.emplace_back(
            facePos, texId, 0,
            checkVertexAO(pos, faceAODirs[0], faceAODirs[1], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + v, texId, 2,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + u, texId, 1,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));

        m_vertices.emplace_back(
            facePos + u, texId, 1,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + v, texId, 2,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + u + v, texId, 3,
            checkVertexAO(pos, faceAODirs[4], faceAODirs[3], faceAODirs[5]));
    }
    else
    {
        m_vertices.emplace_back(
            facePos, texId, 0,
            checkVertexAO(pos, faceAODirs[0], faceAODirs[1], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + u, texId, 1,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + v, texId, 2,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));

        m_vertices.emplace_back(
            facePos + v, texId, 2,
            checkVertexAO(pos, faceAODirs[6], faceAODirs[5], faceAODirs[7]));
        m_vertices.emplace_back(
            facePos + u, texId, 1,
            checkVertexAO(pos, faceAODirs[2], faceAODirs[1], faceAODirs[3]));
        m_vertices.emplace_back(
            facePos + u + v, texId, 3,
            checkVertexAO(pos, faceAODirs[4], faceAODirs[3], faceAODirs[5]));
    }
}

void Chunk::draw(const std::shared_ptr<Renderer> &renderer,
                 const Shader &shader, bool drawOutline)
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
    if (m_meshType == MeshType::CULLED || m_meshType == MeshType::GREEDY)
    {
        shader.setUniform1b("u_bUseAO", renderer->ambientOcclusion());
    }

    m_mesh->draw(renderer, shader);
    if (drawOutline)
    {
        renderer->drawOutline(
            ResourceManager::getInstance().getShader(s_chunkOutline), m_pos);
    }
}

void Chunk::setMesh(const std::shared_ptr<Mesh> &mesh)
{
    m_mesh = mesh;
    m_mesh->reserveMesh();
}

void Chunk::bake(MeshType type)
{
    std::unique_lock<std::shared_mutex> uLock(m_mutex);
    m_state = ChunkState::MESH_BAKING;
    uLock.unlock();

    m_meshType = type;

    switch (type)
    {
    case MeshType::CULLED:
        bakeCulled(m_voxels, g_sAxis);
        break;

    case MeshType::GREEDY:
        bakeGreedy(m_voxels, g_sAxis);
        break;

    case MeshType::BINARY_GREEDY:
        bakeBinGreedy(m_voxels, g_sAxis);
        break;
    }

    uLock.lock();
    if (m_vertices.empty())
    {
        m_state = ChunkState::VOXELS_GENERATED_READY;
    }
    else
    {
        m_state = ChunkState::MESH_BAKED;
    }
}

// @NOTE: This method must work only on the main thread
void Chunk::loadVerticesToBuffer()
{
    std::unique_lock<std::shared_mutex> uLock(m_mutex);
    m_state = LOADING;
    uLock.unlock();

    if (m_mesh == nullptr)
    {
        LOG_CORE_ERROR("CHUNK::MESH::CONSTRUCTION::Can't construct mesh!");
        return;
    }

    if (getState() == ChunkState::READY)
    {
        LOG_CORE_WARN("CHUNK::MESH::CONSTRUCTION::Mesh for this chunks is "
                      "already generated!");
        return;
    }

    m_mesh->loadToBuffer<Vertex>(m_vertices);

    uLock.lock();
    m_state = ChunkState::READY;
}
