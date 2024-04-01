#pragma once

#include <cstdint>
#include <vector>

#include "../Core.hpp"

namespace VoxSmith
{
	enum class VOX_SMITH_API TextureType
	{
		BASE,
		ARRAY,
		PIXEL,
	};

	class VOX_SMITH_API Texture
	{
	public:
		Texture(const char* path);
		Texture(std::vector<const char*> paths);
		Texture(uint32_t width, uint32_t height);

		~Texture();

		inline TextureType getType() const { return m_type; }

		void use() const;

	private:
		uint32_t ID;

		TextureType m_type;
	};
}
