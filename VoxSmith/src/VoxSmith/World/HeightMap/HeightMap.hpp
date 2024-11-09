#include <unordered_map>

#include <glm/glm.hpp>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct VOX_SMITH_API ChunkMap
	{
		std::vector<int32_t> height;
	};

	class VOX_SMITH_API HeightMap
	{
	public:
		HeightMap() = default;
		~HeightMap() = default;

		void update();

		ChunkMap getChunkHeight(const glm::ivec2& pos);

	private:

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
	};
}
