#pragma once

#include <glad/glad.h>

#include <vector>

#include "../Core.hpp"

namespace VoxSmith
{
	struct Vertex;

	enum class VOX_SMITH_API StorageType
	{
		QUAD_TEXTURE,
		MESH,
	};

	class VOX_SMITH_API Buffer final
	{
	public:
		Buffer() = default;
		~Buffer() = default;

		template<typename T, StorageType ST>
		void init(const std::vector<T>& data);

		void bind() const;

	private:
		uint32_t VAO;
		uint32_t VBO;

		bool m_initialized = false;
	};

	template<typename T, StorageType ST>
	void Buffer::init(const std::vector<T>& data)
	{
		if (m_initialized)
		{
			return;
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);

		switch (ST)
		{
		case StorageType::QUAD_TEXTURE: {
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T) * 5, (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(T) * 5, (void*)(sizeof(float) * 3));
		} break;

		case StorageType::MESH: {
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0);
		} break;
		}
	}
}
