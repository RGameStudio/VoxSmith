#include <glm/common.hpp>

#include "VoxSmith/Logger/Log.hpp"

#include "HeightMap.hpp"

using namespace VoxSmith;

constexpr int32_t g_cSize = 32;

const Spline g_baseSpline = {
	{0.0f, 0.1f},
	{0.4f, 0.2f},
	{0.7f, 0.21f},
	{1.0f, 0.32f}
};

const Spline g_mountainAndValleySpline = {
	{0.0f, 0.0f},
	{0.55f, 0.135f},
	{0.65f, 0.835f},
	{0.70f, 0.7956f},
	{0.89f, 0.99563f},
	{1.0f, 1.0f}
};

const Spline g_areaSpline = {
	{0.0f, 0.0f},
	{0.8f, 0.1f},
	{1.0f, 1.0f}
};

HeightMap::HeightMap()
{
	m_base.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_base.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_base.SetFractalOctaves(8);
	m_base.SetFrequency(0.0131791f);

	m_mountains.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_mountains.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_mountains.SetFractalOctaves(6);
	m_mountains.SetFrequency(0.001365482f);

	m_area.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	m_area.SetFractalType(FastNoiseLite::FractalType_None);
	m_area.SetFractalOctaves(6);
	m_area.SetFrequency(0.00122421f);
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

	auto fade = [](float t) {
		return t * t * (3.0f - 2.0f * t);
	};

	for (int32_t z = pos.y; z < pos.y + g_cSize; z++)
	{
		for (int32_t x = pos.x; x < pos.x + g_cSize; x++)
		{
			const float x_ = static_cast<float>(x);
			const float z_ = static_cast<float>(z);

			float baseNoise = m_base.GetNoise(x_, z_) * 0.5f + 0.5f;
			baseNoise = getSplineValue(g_baseSpline, baseNoise);

			float mountainNoise = m_mountains.GetNoise(x_, z_) * 0.5f + 0.5f;
			mountainNoise = getSplineValue(g_mountainAndValleySpline, mountainNoise);

			float areaNoise = m_area.GetNoise(x_, z_) * 0.5f + 0.5f;
			areaNoise = getSplineValue(g_areaSpline, areaNoise);
			
			float height = 100.0f * mountainNoise; //* (baseNoise * areaNoise + (1.0f - areaNoise) * mountainNoise);
			

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
		return (3.0f - 2.0f * t) * t * t;
	};

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
