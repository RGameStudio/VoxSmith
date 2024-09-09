#pragma once

#include <glad/glad.h>

#include <vector>

#include "../Core.hpp"

namespace VoxSmith
{
	struct Vertex;

	class VOX_SMITH_API Buffer final
	{
	public:
		Buffer() = default;
		~Buffer() = default;

		void init(const std::vector<float>& data);
		void init(const std::vector<Vertex>& data);
		void init(const std::vector<float>& data, const std::vector<uint32_t>& indicies);

		void use() const;

	private:
		uint32_t VAO = 0;
		uint32_t VBO = 0;

		uint32_t IBO = 0;

		bool m_initialized = false;
	};
}
