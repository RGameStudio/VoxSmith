#include <glad/glad.h>

#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

using namespace VoxSmith;

Renderer::Renderer()
{

}

void Renderer::draw(const Buffer& buffer, const Shader& shader) const
{
	shader.use();
	glBindVertexArray(buffer.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::draw(const Buffer& buffer, const Shader& shader, const Texture& texture) const
{
	shader.use();
	texture.use();
	glBindVertexArray(buffer.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void VoxSmith::initQuadBuffer(Buffer& buffer, const std::vector<float>& data)
{
	glGenVertexArrays(1, &buffer.VAO);
	glGenBuffers(1, &buffer.VBO);

	glBindVertexArray(buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
}

void VoxSmith::initMeshBuffer(Buffer& buffer, const std::vector<Vertex>& data)
{
	glGenVertexArrays(1, &buffer.VAO);
	glGenBuffers(1, &buffer.VBO);

	glBindVertexArray(buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
}
