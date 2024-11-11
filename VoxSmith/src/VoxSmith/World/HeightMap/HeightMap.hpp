#include <FastNoiseLite.h>
#include <unordered_map>

#include <glm/glm.hpp>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct VOX_SMITH_API ChunkMap
	{
		ChunkMap() = default;
		ChunkMap(const ChunkMap&) = default;
		ChunkMap& operator=(const ChunkMap&) = default;
		ChunkMap(ChunkMap&&) = default;

		std::vector<int32_t> data;
	};

	class VOX_SMITH_API HeightMap
	{
	public:
		HeightMap();
		~HeightMap() = default;

		ChunkMap& getChunkMap(const glm::ivec2& pos);

	private:
		void generateMap(ChunkMap& map, const glm::ivec2& pos);

		struct KeyFuncs
		{
			size_t operator()(const glm::ivec2& v) const
			{
				size_t h = 0xcf234123f;
				h ^= std::_Bit_cast<int32_t>(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
				h ^= std::_Bit_cast<int32_t>(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
				return h;
			}

			bool operator()(const glm::ivec2& a, const glm::ivec2& b) const
			{
				return a.x == b.x && a.y == b.y;
			}
		};

		std::unordered_map<glm::ivec2, ChunkMap, KeyFuncs> m_map;
		FastNoiseLite m_baseNoiseGen;
		FastNoiseLite m_mountainNoiseGen;
	};
}
