#pragma once

#include <vector>

#include <VoxSmith/Core.hpp>

namespace VoxSmith
{
	class Chunk;

	class VOX_SMITH_API ChunkManager
	{
	public:

	private:
		std::vector<Chunk> m_renderList;
		std::vector<Chunk> m_updateList;
	};
}