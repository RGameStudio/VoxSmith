#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "VoxSmith/Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

void Mesh::draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const
{
	if (renderer == nullptr)
	{
		return;
	}

	renderer->draw(m_buffer, shader, m_vertexCount);
}

void Mesh::loadToBuffer(const std::vector<Vertex>& vertices)
{
	m_vertexCount = vertices.size();
	m_buffer.init(vertices);
	m_isConstructed = true;
}
