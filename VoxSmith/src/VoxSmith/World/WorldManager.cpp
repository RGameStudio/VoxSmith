#include "VoxSmith/Chunk/Voxel.hpp"
#include "VoxSmith/Logger/Log.hpp"

#include "World.hpp"
#include "WorldManager.hpp"

using namespace VoxSmith;

WorldManager* WorldManager::s_manager = nullptr;

WorldManager::~WorldManager()
{
    delete s_manager;
}

WorldManager& WorldManager::getInstance()
{
    if (s_manager == nullptr)
    {
        s_manager = new WorldManager();
    }

    return *s_manager;
}

void WorldManager::init(World* world)
{
    if (m_world == nullptr)
    {
        m_world = world;
    }
}

VoxelType WorldManager::getVoxel(const glm::vec3& pos) const
{
    if (m_world == nullptr)
    {
        LOG_CORE_ERROR("WORLD_MANAGER::GET_VOXEL::World wasn't initialized");
        return VoxelType::Empty;
    }

    return m_world->getVoxel(pos);
}