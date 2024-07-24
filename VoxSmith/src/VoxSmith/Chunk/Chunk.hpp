#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Voxel.hpp"

#include "../Core.hpp"

namespace VoxSmith
{
	class Renderer;
	class Shader;
	struct Vertex;

	class VOX_SMITH_API Chunk final
	{
	public:
		Chunk(const glm::vec3& pos);
		~Chunk() = default;

		void setMesh(const std::shared_ptr<Mesh>& mesh);
		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const;
		void constructMesh();

	private:
		glm::vec3 m_pos;

		std::shared_ptr<Mesh> m_mesh = nullptr;
		
		std::vector<Voxel> m_voxels;
		std::vector<Vertex> m_vertices;

		enum Direction : int8_t
		{
			LEFT	= 0,
			BOTTOM	= 1,
			BACK	= 2, 
			RIGHT	= 3,
			TOP		= 4, 
			FRONT	= 5
		};

		std::vector<std::shared_ptr<Chunk>> m_neighbours;

		enum FaceType : int8_t
		{
			None = -1,
			BackFace = 0,
			FrontFace = 1,
		};

		int32_t getId(const glm::vec3& v, const float cSize);

		void bakeStupid(const std::vector<Voxel>& voxels, const float size);
		void bakeCulled(const std::vector<Voxel>& voxels, const float size);
		void bakeGreedy(const std::vector<Voxel>& voxels, const float size);
		void bakeBinGreedy(const std::vector<Voxel>& voxels, const float size);

		void addQuadFace(const glm::vec3& pos, const glm::vec3& u, const glm::vec3& v);
		void defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const FaceType face, const int32_t iAxis) const;
	};
}