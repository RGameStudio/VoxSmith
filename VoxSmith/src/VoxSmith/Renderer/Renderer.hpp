#pragma once

#include <cstdint>
#include <vector>

#include "../Core.hpp"

namespace VoxSmith
{
	struct VOX_SMITH_API Buffer
	{
		uint32_t VAO;
		uint32_t VBO;
	};

	class Shader;
	class Texture;

	class VOX_SMITH_API Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		void draw(const Buffer& buffer, const Shader& shader);
		void draw(const Buffer& buffer, const Shader& shader, const Texture& texture);

	private:
	};

	void VOX_SMITH_API initBuffer(Buffer& buffer, const std::vector<float>& data);
}