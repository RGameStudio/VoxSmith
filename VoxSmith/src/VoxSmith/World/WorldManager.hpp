#pragma once

#include <glm/glm.hpp>

#include "VoxSmith/Chunk/Voxel.hpp"
#include "VoxSmith/Core.hpp"

namespace VoxSmith 
{
    class World;

    class VOX_SMITH_API WorldManager final
    {
    public:
        static WorldManager& getInstance();
        void init(World* world);

        VoxelType getVoxel(const glm::vec3& pos) const;

    private:
        WorldManager() = default;
        ~WorldManager();

        static WorldManager* s_manager;
        World* m_world;
    };
}