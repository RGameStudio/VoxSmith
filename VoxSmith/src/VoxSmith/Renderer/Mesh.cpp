#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "../Chunk/Voxel.hpp"

#include "Mesh.hpp"

using namespace VoxSmith;

void Mesh::bakeGreedyMesh(const std::vector<Voxel>& voxels, const glm::vec3& pos)
{
	Vertex v000 = { pos };
	Vertex v100 = { pos + glm::vec3(1, 0, 0) };
	Vertex v010 = { pos + glm::vec3(0, 1, 0) };
	Vertex v001 = { pos + glm::vec3(0, 0, 1) };
	Vertex v101 = { pos + glm::vec3(1, 0, 1) };
	Vertex v011 = { pos + glm::vec3(0, 1, 1) };
	Vertex v110 = { pos + glm::vec3(1, 0, 1) };
	Vertex v111 = { pos + glm::vec3(1, 1, 1) };

	m_vertices.push_back(v000);
	m_vertices.push_back(v100);
	m_vertices.push_back(v010);
	m_vertices.push_back(v001);
	m_vertices.push_back(v101);
	m_vertices.push_back(v011);
	m_vertices.push_back(v110);
	m_vertices.push_back(v111);
}

void Mesh::draw(const Renderer& renderer, const Shader& shader) const
{
	renderer.draw(m_buffer, shader);
}

void Mesh::loadToBuffer()
{
	m_buffer.init(m_vertices);
}
