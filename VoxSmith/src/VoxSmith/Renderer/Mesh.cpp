#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "../Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

void Mesh::bakeGreedy(const std::vector<Voxel>& voxels, const glm::vec3& pos, const glm::vec3& size)
{

}

void Mesh::bakeBinaryGreedy(const std::vector<Voxel>& voxels, const glm::vec3& pos, const glm::vec3& size)
{

}

void Mesh::draw(const Renderer& renderer, const Shader& shader) const
{
	renderer.draw(m_buffer, shader);
}

void Mesh::loadToBuffer()
{
	m_buffer.init(m_vertices);
}
