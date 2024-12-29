#pragma once

#include <cstdint>
#include <vector>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	enum class VOX_SMITH_API TextureType
	{
		BASE,
		ARRAY,
		PIXEL,
	};

	class VOX_SMITH_API Texture final
	{
	public:
		Texture(const char* path);
		Texture(std::vector<std::string> paths);
		Texture(const uint32_t width, const uint32_t height);

		~Texture();

		inline TextureType getType() const { return m_type; }

		void use() const;

		Texture() = default;

	private:
		uint32_t ID;

		TextureType m_type;
	};
}
