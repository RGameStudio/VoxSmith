#include <glm/common.hpp>

#include "VoxSmith/Logger/Log.hpp"

#include "HeightMap.hpp"

using namespace VoxSmith;

constexpr int32_t g_cSize = 32;

HeightMap::HeightMap()
{
    m_baseNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_baseNoiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_baseNoiseGen.SetFractalOctaves(7);
    m_baseNoiseGen.SetFrequency(0.001f);

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

            const float baseNoise = m_baseNoiseGen.GetNoise(x_, z_);
            float biomeNoise = m_biomNoiseGen.GetNoise(x_, z_);
 
            float desertWeight = glm::smoothstep(-1.0f, -0.3f, biomeNoise);
            float meadowWeight = glm::smoothstep(-0.3f, 0.3f,biomeNoise);
            float mountainWeight = glm::smoothstep(0.25f, 1.0f, biomeNoise);
            
            Biome biome;

            if (biomeNoise <= -0.3f)
            {
                biome.topVoxel = Voxel::Sand;
                biome.mainVoxel = Voxel::Sand;
            }
            else if (biomeNoise <= 0.25f)
            {
                biome.topVoxel = Voxel::Grass;
                biome.mainVoxel = Voxel::Dirt;
            }
            else if (biomeNoise >= 0.3f)
            {
                biome.topVoxel = Voxel::Stone;
                biome.mainVoxel = Voxel::Stone;
            }
            else
            {
                const float heightLeft = 100.0f + (30.0f * m_featureNoiseGen.GetNoise(x_ - 1, z_));
                const float heightRight = 100.0f + (30.0f * m_featureNoiseGen.GetNoise(x_ + 1, z_));
                const float heightDown = 100.0f + (30.0f * m_featureNoiseGen.GetNoise(x_, z_ - 1));
                const float heightUp = 100.0f + (30.0f * m_featureNoiseGen.GetNoise(x_, z_ + 1));

                float slope = glm::abs(heightLeft - heightRight) + glm::abs(heightDown - heightUp);
                slope *= 10.0f;
                const float slopeThreshold = 0.5;

                if (slope > slopeThreshold)
                {
                    biome.topVoxel = Voxel::GrassMount;
                    biome.mainVoxel = Voxel::Dirt;
                }
                else
                {
                    biome.topVoxel = Voxel::Grass;
                    biome.mainVoxel = Voxel::Dirt;
                }
            }

            float desertHeight = 5.0f + 10.0f * m_featureNoiseGen.GetNoise(x_, z_);
            float meadowHeight = 10.0f + 50.0f * m_featureNoiseGen.GetNoise(x_, z_);
            float mountainHeight = 100.0f + 200.0f * m_featureNoiseGen.GetNoise(x_, z_);
            
            const float blendHeight =
                meadowHeight * meadowWeight +
                mountainHeight * mountainWeight;

            map.data.push_back(100.0f + (baseNoise * 5.0f) + blendHeight);
            map.biomData.push_back(biome);
        }
    }

    return map;
}

