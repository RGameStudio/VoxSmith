#include <glm/common.hpp>

#include "VoxSmith/Logger/Log.hpp"

#include "HeightMap.hpp"

using namespace VoxSmith;

constexpr int32_t g_cSize = 32;

const Spline g_baseSpline = {
	{0.0f, 0.0f},
	{0.5f, 0.2f},
	{0.6f, 0.85f},
	{0.7f, 0.95f},
	{1.0f, 1.0f}
};

const Spline g_mountainAndValleySpline = {
	{0.0f, 0.0f},
	{0.3f, 0.1f},
	{0.5f, 0.3f},
	{0.5f, 0.96f},
	{1.0f, 1.0f}
};

HeightMap::HeightMap()
{
	m_base.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_base.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_base.SetFractalOctaves(8);
	m_base.SetFrequency(0.00131791f);

	m_mountains.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_mountains.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_mountains.SetFractalOctaves(7);
	m_mountains.SetFrequency(0.0001365482f);
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

			float baseNoise = m_base.GetNoise(x_, z_);
			baseNoise = (baseNoise + 1.0f) / 2.0f;

			float mountainNoise = m_mountains.GetNoise(x_, z_);
			mountainNoise = (mountainNoise + 1.0f) / 2.0f;
			
			const float height = 120.0f * getSplineValue(g_baseSpline, baseNoise);

			Biome biome;
			biome.mainVoxel = VoxelType::Dirt;
			biome.topVoxel = VoxelType::Grass;

			map.data.push_back(100.0f + height);
			map.biomData.push_back(biome);
		}
	}

	return map;
}

float HeightMap::getSplineValue(const Spline& spline, float t) const
{
	auto fade = [](const float t)
	{
		return 3 * t * t - 2 * t * t * t;
	};

	for (uint32_t iPoint = 0; iPoint < spline.size() - 1; iPoint++)
	{
		if (spline[iPoint].x <= t && t <= spline[iPoint + 1].x)
		{
#if 1
			const float out = (t - spline[iPoint].x) / (spline[iPoint + 1].x - spline[iPoint].x);
			return glm::mix(spline[iPoint].y, spline[iPoint + 1].y, out);
#else

#endif
		}
	}

	return spline.back().y;
}
