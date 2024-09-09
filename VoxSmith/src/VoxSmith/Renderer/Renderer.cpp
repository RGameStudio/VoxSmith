#include <vector>

#include <glad/glad.h>

#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Texture/Texture.hpp"

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

using namespace VoxSmith;

const std::vector<float> g_chunkOutlineVerts = {
	 0,  0,  0, // -x -y -z (0)
	 0, 32,  0, // -x +y -z (1)
	32, 32,  0, // +x +y -z (2)
	32,  0,  0, // +x -y -z (3)
	 0,  0, 32, // -x -y +z (4)
	 0, 32, 32, // -x +y +z (5)
	32, 32, 32, // +x +y +z (6)
	32,  0, 32, // +x -y +z (7)
};

const std::vector<uint32_t> g_cubeOutlineInds = {
	0, 1,	1, 2,	2, 3,	3, 0,	// front
	4, 5,	5, 6,	6, 7,	7, 4,	// back
	0, 4,	1, 5,					// left
	3, 7,	2, 6					// right
};

Renderer::Renderer()
{
	glEnable(GL_CULL_FACE);
	m_outlineBuff.init(g_chunkOutlineVerts, g_cubeOutlineInds);
}

void Renderer::draw(const Buffer& buffer, const Shader& shader, const int32_t count) const
{
	shader.use();
	buffer.use();
	
	auto mode = GL_TRIANGLES;
	if (m_showEdges)
	{
		mode = GL_LINES;
	}

	glDrawArrays(mode, 0, count);
}

void Renderer::drawOutline(const Shader& shader, const glm::vec3& pos) const
{
	shader.use();
	shader.setUniform3fv("u_chunkPos", pos);
	m_outlineBuff.use();
	

	auto mode = GL_LINES;
	
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
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
