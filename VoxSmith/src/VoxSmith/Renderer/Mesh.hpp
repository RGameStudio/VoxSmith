#pragma once

#include <vector>

#include "../Core.hpp"

namespace VoxSmith
{
	struct Vertex
	{
		int16_t data;
	};

	class VOX_SMITH_API Mesh
	{
	public:
		Mesh();
		~Mesh();
	};
}