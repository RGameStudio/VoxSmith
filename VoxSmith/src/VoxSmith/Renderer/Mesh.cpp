#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"
#include "VoxSmith/Shader/Shader.hpp"

#include "VoxSmith/Chunk/Voxel.hpp"

#include "Mesh.hpp"  

using namespace VoxSmith;

void Mesh::draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const
{
	if (renderer == nullptr)
	{
		LOG_CORE_ERROR("ERROR::MESH::DRAW::Passed renderer is not initialized!");
		return;
	}

	renderer->draw(m_buffer, shader, m_vertexCount);
}
