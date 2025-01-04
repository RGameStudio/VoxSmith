#pragma once

#include <glad/glad.h>

#include <vector>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct Vertex;

	class VOX_SMITH_API Buffer final
	{
	public:
		Buffer() = default;
		~Buffer() = default;

		void init(const std::vector<float>& data, const uint32_t mode = GL_STATIC_DRAW);
		void init(const std::vector<Vertex>& data, const uint32_t mode = GL_STATIC_DRAW);
		void init(const std::vector<float>& data, const std::vector<uint32_t>& indicies, const uint32_t mode = GL_STATIC_DRAW);

		void use() const;

	private:
		uint32_t VAO = 0;
		uint32_t VBO = 0;

		uint32_t IBO = 0;

		bool m_initialized = false;
	};
}
