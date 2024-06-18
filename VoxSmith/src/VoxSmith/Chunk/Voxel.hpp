#pragma once

#include "../Core.hpp"

namespace VoxSmith
{
	enum VOX_SMITH_API VoxelType
	{
		EMPTY = 0,
		OPAQUE = 1,
	};

	struct VOX_SMITH_API Voxel final
	{
		VoxelType type;
	};
}