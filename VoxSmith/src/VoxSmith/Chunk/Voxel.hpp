#pragma once

#include <unordered_map>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	enum class VOX_SMITH_API VoxelType : int8_t
	{
		Empty,
		Dirt,
		Grass,
		Stone,
	};

	using Voxel = VoxelType;

	static std::unordered_map<VoxelType, glm::u8vec3> s_voxelColors = {
		{ VoxelType::Dirt,  { 155, 118, 83  } },
		{ VoxelType::Grass, { 0,   255, 0   } },
		{ VoxelType::Stone, { 183, 176, 156 } },
	};

}