#pragma once

#include <cstdint>
#include <vector>

#include "Buffer.hpp"

#include "../Core.hpp"

namespace VoxSmith
{
	class Shader;
	class Texture;
	struct Vertex;

	class VOX_SMITH_API Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		void draw(const Buffer& buffer, const Shader& shader) const;
		void draw(const Buffer& buffer, const Shader& shader, const Texture& texture) const;

	private:
	};
}
