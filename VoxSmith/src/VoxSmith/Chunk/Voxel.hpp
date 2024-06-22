#pragma once

#include "../Core.hpp"

namespace VoxSmith
{
	enum VOX_SMITH_API VoxelType
	{
		EMPTY,
		OPAQUE,
	};

	struct VOX_SMITH_API Voxel final
	{
		VoxelType type;
	};
}