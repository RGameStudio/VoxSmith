#include "VoxSmith/Logger/Log.hpp"

#include "HeightMap.hpp"

using namespace VoxSmith;

constexpr uint32_t g_cSize = 32;

HeightMap::HeightMap()
{
	m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_baseNoiseGen.SetFractalOctaves(8);
	m_baseNoiseGen.SetFrequency(0.00082f);
	m_baseNoiseGen.SetFractalLacunarity(2.1f);

	m_mountainNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_mountainNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_mountainNoiseGen.SetFractalOctaves(8);
	m_mountainNoiseGen.SetFrequency(0.00165f);
	m_mountainNoiseGen.SetFractalLacunarity(1.85f);
}

ChunkMap& HeightMap::getChunkMap(const glm::ivec2& pos)
{
	if (m_map.find(pos) == m_map.end())
	{
		ChunkMap map;
		generateMap(map, pos);
		m_map[pos] = map;
	}

	return m_map[pos];
}

void HeightMap::generateMap(ChunkMap& map, const glm::ivec2& pos)
{
	auto lerp = [](const float a, const float b, const float t) {
		return a * (1 - t) + b * t;
	};

	for (int32_t z = pos.y; z < pos.y + g_cSize; z++)
	{
		for (int32_t x = pos.x; x < pos.x + g_cSize; x++)
		{
			const float xFloat = static_cast<float>(x);
			const float zFloat = static_cast<float>(z);

			const float n1 = 0.15f * m_baseNoiseGen.GetNoise(xFloat, zFloat);
			const float n2 = 1.6f * m_mountainNoiseGen.GetNoise(xFloat, zFloat);

			const float coeff = 300;

			float t;
			if (n2 > n1)
			{
				t = n2 - n1;
				t *= t;
				t = 1.0f - (1 - t) * (1 - t);
			}
			const float noiseFactor = n2 > n1 ? lerp(n1, n2, t) : n1;

			map.data.push_back(128 +
				coeff * (noiseFactor));
		}
	}
}
