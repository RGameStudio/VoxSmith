#pragma once

#include <vector>
#include <memory>

#include <VoxSmith/Core.hpp>

namespace VoxSmith
{
	class Chunk;

	class VOX_SMITH_API ChunkManager
	{
	public:

	private:
		std::vector<std::shared_ptr<Chunk>> m_renderList;
		std::vector<std::shared_ptr<Chunk>> m_updateList;
	};
}