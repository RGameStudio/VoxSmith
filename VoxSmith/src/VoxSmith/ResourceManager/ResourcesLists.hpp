#pragma once

#include <unordered_map>
#include <string>
#include <vector>

namespace VoxSmith
{
	static const std::string s_mesh = "mesh";
	static const std::string s_chunkOutline = "chunk_outline";
	static const std::string s_voxelTextureList = "texture_list";

	static const std::unordered_map<std::string, std::string> s_shaderPaths = {
		{s_mesh, "Shaders/mesh_basic.glsl"},
		{s_chunkOutline, "Shaders/chunk_outline.glsl"},
	};

	static const std::vector<std::string> s_texturePaths = {
		"Resources/textures/dirt.png",
		"Resources/textures/dirt_grass.png",
		"Resources/textures/grass.png",
		"Resources/textures/sand.png",
		"Resources/textures/snow.png",
		"Resources/textures/snow_2.png",
		"Resources/textures/dirt.png",
		"Resources/textures/water.png",
	};
}