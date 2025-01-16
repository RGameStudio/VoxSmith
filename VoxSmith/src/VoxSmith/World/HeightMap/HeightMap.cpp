#include <glm/common.hpp>

#include "VoxSmith/Logger/Log.hpp"

#include "HeightMap.hpp"

using namespace VoxSmith;

constexpr int32_t g_cSize = 32;

const Spline baseSpline = {
	{0.0f, 0.0f},
	{0.5f, 0.2f},
	{0.6f, 0.3f},
	{0.7f, 0.95f},
	{1.0f, 1.0f}
};

HeightMap::HeightMap()
{
	m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_baseNoiseGen.SetFractalOctaves(8);
	m_baseNoiseGen.SetFrequency(0.00131791f);

	m_biomNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_biomNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_biomNoiseGen.SetFractalOctaves(8);
	m_biomNoiseGen.SetFrequency(0.0001931f);

	m_featureNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_featureNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_featureNoiseGen.SetFractalOctaves(9);
	m_featureNoiseGen.SetFrequency(0.001723f);
}

ChunkMap& HeightMap::getChunkMap(const glm::ivec2& pos)
{
	if (m_map.find(pos) == m_map.end())
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_map[pos] = generateMap(pos);
	}

	return m_map[pos];
}

ChunkMap HeightMap::generateMap(const glm::ivec2& pos)
{
	ChunkMap map;

	for (int32_t z = pos.y; z < pos.y + g_cSize; z++)
	{
		for (int32_t x = pos.x; x < pos.x + g_cSize; x++)
		{
			const float x_ = static_cast<float>(x);
			const float z_ = static_cast<float>(z);

			float baseNoise = m_baseNoiseGen.GetNoise(x_, z_);
			baseNoise = (baseNoise + 1.0f) / 2.0f;
			
			const float height = 100.0f * getSplineValue(baseSpline, baseNoise);

			Biome biome;
			biome.mainVoxel = VoxelType::Dirt;
			biome.topVoxel = VoxelType::Grass;

			map.data.push_back(100.0f + height);
			map.biomData.push_back(biome);
		}
	}

	return map;
}

float HeightMap::getSplineValue(const Spline& spline, float t)
{
	for (uint32_t iPoint = 0; iPoint < spline.size() - 1; iPoint++)
	{
		if (spline[iPoint].x <= t && t <= spline[iPoint + 1].x)
		{
			const float out = (t - spline[iPoint].x) / (spline[iPoint + 1].x - spline[iPoint].x);
			return glm::mix(spline[iPoint].y, spline[iPoint + 1].y, out);
		}
	}

	return spline.back().y;
}
