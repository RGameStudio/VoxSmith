#pragma once

#include <unordered_map>

#include "../Core.hpp"

namespace VoxSmith
{
	enum class VOX_SMITH_API VoxelType : int8_t
	{
		Empty,
		Dirt,
		Grass,
	};

	using Voxel = VoxelType;

	static std::unordered_map<VoxelType, glm::vec3> s_voxelColors = {
		{VoxelType::Dirt, {0.57f, 0.42f, 0.3f}},
		{VoxelType::Grass, {0, 1.0f, 0.0f}},
	};

}