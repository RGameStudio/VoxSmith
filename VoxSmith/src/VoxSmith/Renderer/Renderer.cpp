#include <glad/glad.h>

#include "../Shader/Shader.hpp"

#include "Renderer.hpp"

using namespace VoxSmith;

Renderer::Renderer()
{

}

void Renderer::draw(const Buffer& buffer, const Shader& shader)
{
	shader.use();
	glBindVertexArray(buffer.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void VoxSmith::initBuffer(Buffer& buffer, const std::vector<float>& data)
{
	glGenVertexArrays(1, &buffer.VAO);
	glGenBuffers(1, &buffer.VBO);

	glBindVertexArray(buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}
