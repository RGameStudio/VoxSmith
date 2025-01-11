#pragma once

#include "VoxSmith/Core.hpp"

#include "VoxSmith/ResourceManager/ResourcesLists.hpp"

#include <unordered_map>
#include <array>

namespace VoxSmith
{
	enum class VOX_SMITH_API VoxelType : int8_t
	{
		Empty = -1,
		Dirt,
		Grass,
		GrassMount,
		Stone,
		Sand
	};

	enum VOX_SMITH_API Direction : int8_t
	{
		LEFT = 0,
		BOTTOM,
		BACK,
		RIGHT,
		TOP,
		FRONT,
	};

	using Voxel = VoxelType;

	static std::unordered_map<VoxelType, glm::u8vec3> s_voxelColors = {
		{ VoxelType::Dirt,  { 155, 118, 83  } },
		{ VoxelType::Grass, { 0,   255, 0   } },
		{ VoxelType::Stone, { 183, 176, 156 } },
	};

	static std::unordered_map<VoxelType, std::array<TextureFace, 6>> s_textureFaces = {
		{ 
			VoxelType::Dirt,  
			{ 
				TextureFace::DIRT, 
				TextureFace::DIRT, 
				TextureFace::DIRT, 
				TextureFace::DIRT, 
				TextureFace::DIRT,
				TextureFace::DIRT,
			} },
		{ 
			VoxelType::Grass, 
			{ 
				TextureFace::DIRT_GRASS, 
				TextureFace::DIRT, 
				TextureFace::DIRT_GRASS,
				TextureFace::DIRT_GRASS,
				TextureFace::GRASS, 
				TextureFace::DIRT_GRASS 
			} 
		},
		{
			VoxelType::GrassMount,
			{
				TextureFace::DIRT_GRASS_MOUNT,
				TextureFace::DIRT_MOUNT,
				TextureFace::DIRT_GRASS_MOUNT,
				TextureFace::DIRT_GRASS_MOUNT,
				TextureFace::GRASS_MOUNT,
				TextureFace::DIRT_GRASS_MOUNT
			}
		},
		{
			VoxelType::Stone,
			{
				TextureFace::STONE,
				TextureFace::STONE,
				TextureFace::STONE,
				TextureFace::STONE,
				TextureFace::STONE,
				TextureFace::STONE
			}
		},
		{
			VoxelType::Sand,
			{
				TextureFace::SAND,
				TextureFace::SAND,
				TextureFace::SAND,
				TextureFace::SAND,
				TextureFace::SAND,
				TextureFace::SAND
			}
		}
	};
}