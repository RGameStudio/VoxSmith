#include <glad/glad.h>

#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

using namespace VoxSmith;

Renderer::Renderer()
{
	glEnable(GL_CULL_FACE);
}

void Renderer::draw(const Buffer& buffer, const Shader& shader, const int32_t count) const
{
	shader.use();
	buffer.use();
	
	auto mode = GL_TRIANGLES;
	if (m_showEdges)
	{
		// mode = GL_LINE;
		mode = GL_LINE_LOOP;
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

void Renderer::switchCulling() const
{
	if (m_cullingStatus)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}
