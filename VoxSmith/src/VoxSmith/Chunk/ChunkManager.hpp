#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>

#include <VoxSmith/Core.hpp>

namespace VoxSmith
{
	class Chunk;

	class VOX_SMITH_API ChunkManager
	{
	public:
		static ChunkManager& getChunkManager();

	private:
		// ChunkManager() = default;
		// ~ChunkManager() = default;

		static std::unique_ptr<ChunkManager> s_chunkManager;
		std::vector<std::shared_ptr<Chunk>> m_updateList;
	};
}