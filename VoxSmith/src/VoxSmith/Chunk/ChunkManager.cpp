#include "ChunkManager.hpp"

using namespace VoxSmith;

std::unique_ptr<ChunkManager> ChunkManager::s_chunkManager = nullptr;

ChunkManager& ChunkManager::getChunkManager()
{
	if (s_chunkManager == nullptr)
	{
		s_chunkManager = std::make_unique<ChunkManager>();
	}

	return *s_chunkManager;
}
