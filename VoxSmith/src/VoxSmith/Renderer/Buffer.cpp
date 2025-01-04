#include "Mesh.hpp"
#include "Buffer.hpp"

using namespace VoxSmith;

void Buffer::use() const
{
	glBindVertexArray(VAO);
}

void Buffer::init(const std::vector<float>& data, const uint32_t mode)
{
	if (m_initialized)
	{
		return;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), mode);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	m_initialized = true;
}

void Buffer::init(const std::vector<Vertex>& vertices, const uint32_t mode)
{
	if (m_initialized)
	{
		return;
	}
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), mode);

	glEnableVertexAttribArray(0); 
#if 1
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, portion1));
#else
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
#endif
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, portion2));

	m_initialized = true;
}

void Buffer::init(const std::vector<float>& data, const std::vector<uint32_t>& indicies, const uint32_t mode)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), mode);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(float), indicies.data(), mode);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glLineWidth(3);
}
