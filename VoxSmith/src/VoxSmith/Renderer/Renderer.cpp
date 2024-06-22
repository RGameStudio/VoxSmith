#include <glad/glad.h>

#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

using namespace VoxSmith;

void Renderer::draw(const Buffer& buffer, const Shader& shader) const
{
	shader.use();
	buffer.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::draw(const Buffer& buffer, const Shader& shader, const Texture& texture) const
{
	shader.use();
	buffer.use();
	texture.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
