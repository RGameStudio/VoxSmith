#pragma once

#include "../Core.hpp"

namespace VoxSmith
{
	enum class VOX_SMITH_API VoxelType : int8_t
	{
		Empty,
		Opaque
	};

	using Voxel = VoxelType;
}