#pragma once

#include "VoxSmith/Core.hpp"

#include <unordered_map>
#include <string>
#include <vector>

namespace VoxSmith
{
	static const std::string s_mesh = "mesh";
	static const std::string s_meshTexture = "mesh_texture";
	static const std::string s_chunkOutline = "chunk_outline";
	static const std::string s_voxelTextureList = "texture_list";

	static const std::unordered_map<std::string, std::string> s_shaderPaths = {
		{s_mesh, "Resources/Shaders/mesh_basic.glsl"},
		{s_chunkOutline, "Resources/Shaders/chunk_outline.glsl"},
		{s_meshTexture, "Resources/Shaders/mesh_basic_texture.glsl"}
	};

	enum VOX_SMITH_API TextureFace : int8_t
	{
		DIRT = 0,
		GRASS,
		DIRT_GRASS,
		SAND,
		SNOW,
		SNOW_2,
		WATER,
		STONE,
		GRASS_MOUNT,
		DIRT_GRASS_MOUNT,
		DIRT_MOUNT,
	};
	
	static const std::vector<std::string> s_texturePaths = {
		"Resources/textures/dirt.png",					// 0
		"Resources/textures/grass.png",					// 1
		"Resources/textures/dirt_grass.png",			// 2
		"Resources/textures/sand.png",					// 3
		"Resources/textures/snow.png",					// 4
		"Resources/textures/snow_2.png",				// 5
		"Resources/textures/water.png",					// 6
		"Resources/textures/stone.png",					// 7
		"Resources/textures/grass_mountain.png",		// 8
		"Resources/textures/dirt_grass_mountain.png",	// 9
		"Resources/textures/dirt_mountain.png"
	};
}