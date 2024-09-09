#pragma once

#include "unordered_map"
#include "string"

namespace VoxSmith
{
	static const std::string s_mesh = "mesh";
	static const std::string s_chunkOutline = "chunk_outline";

	static const std::unordered_map<std::string, std::string> s_shaderPaths = {
		{s_mesh, "shaders/mesh_basic.glsl"},
		{s_chunkOutline, "shaders/chunk_outline.glsl"}
	};

}