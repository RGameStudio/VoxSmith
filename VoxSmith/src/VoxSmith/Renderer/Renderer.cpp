#include <glad/glad.h>

#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

using namespace VoxSmith;

void Renderer::draw(const Buffer& buffer, const Shader& shader, const int32_t count) const
{
	shader.use();
	buffer.use();
	
	auto mode = GL_TRIANGLES;
	if (m_showEdges)
	{
		mode = GL_LINE_STRIP_ADJACENCY;
	}

	glDrawArrays(mode, 0, count);
}

void Renderer::draw(const Buffer& buffer, const Shader& shader, const Texture& texture) const
{
	shader.use();
	buffer.use();
	texture.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
