#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <FastNoiseLite.h>
#include <glm/glm.hpp>

#include "VoxSmith/Renderer/Mesh.hpp"
#include "Voxel.hpp"

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	class Renderer;
	class Shader;
	class Mesh;

	enum VOX_SMITH_API Direction : int8_t
	{
		LEFT = 0,
		BOTTOM,
		BACK,
		RIGHT,
		TOP,
		FRONT,	
	};

	enum VOX_SMITH_API ChunkState : int8_t
	{
		EMPTY,
		VOXELS_GENERATING,
		VOXELS_GENERATED,
		MESH_BAKING,
		MESH_BAKED,
		LOADING,
		READY
	};

	class VOX_SMITH_API Chunk final
	{
	public:
		Chunk(const glm::vec3& pos, FastNoiseLite& noiseGenerator, FastNoiseLite& mountainGenerator);

		void addNeighbour(Direction dir, const std::shared_ptr<Chunk>& chunk);
		void setMesh(const std::shared_ptr<Mesh>& mesh);
		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader, bool drawOutline);
		void constructMesh();
		void loadVerticesToBuffer();

		ChunkState getState() const;

		inline bool isMeshConstructed() const { return m_mesh->isMeshConstructed(); }
		inline glm::vec3 getPos() const { return m_pos; }
		
		Chunk() = default;
		~Chunk() = default;
	private:
		glm::vec3 m_pos;

		ChunkState m_state = ChunkState::EMPTY;

		std::shared_ptr<Mesh> m_mesh = nullptr;

		mutable std::mutex m_mutex;

		std::vector<Voxel> m_voxels;
		std::vector<Vertex> m_vertices;

		std::vector<std::shared_ptr<Chunk>> m_neighbours;

		enum FaceType : int8_t
		{
			None = -1,
			BackFace = 0,
			FrontFace = 1,
		};

		int32_t getId(const glm::vec3& v, const float cSize);

		void bakeCulled(const std::vector<Voxel>& voxels, const float size);
		void bakeGreedy(const std::vector<Voxel>& voxels, const float size);
		void bakeBinGreedy(const std::vector<Voxel>& voxels, const float size);

		void addQuadFace(glm::vec3& pos, const int32_t iSide, const int32_t iAxis, const glm::vec3& u, const glm::vec3& v, const glm::vec3& color, const int32_t id);
		void addQuadFace(const glm::vec3& pos, const glm::vec3& u, const glm::vec3& v, const glm::vec3& color, const int32_t id);
		
		void defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const bool backFace, const int32_t iAxis) const;	
	};
}