#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <array>

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
	struct ChunkMap;
	struct Frustum;
	struct Plane;

	Direction getInverseDirection(Direction dir);

	enum VOX_SMITH_API ChunkState : int8_t
	{
		EMPTY = 0,
		FRESH,
		GENERATING,
		VOXELS_GENERATED,
		VOXELS_GENERATED_READY,
		MESH_BAKING,
		MESH_BAKED,
		LOADING,
		READY,
	};

	enum class VOX_SMITH_API MeshType : int8_t
	{
		CULLED,
		GREEDY,
		BINARY_GREEDY,
	};

	class VOX_SMITH_API Chunk final
	{
	public:
		Chunk(const glm::vec3& pos);
		~Chunk();

		void addNeighbour(Direction dir, const std::shared_ptr<Chunk>& chunk);
		void setMesh(const std::shared_ptr<Mesh>& mesh);
		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader, bool drawOutline);
		void bake(MeshType type = MeshType::CULLED);
		void loadVerticesToBuffer();
		void generateChunk(const ChunkMap& map);
		
		bool canBake() const;
		bool inBounds(const glm::ivec3& min, const glm::ivec3& max) const;
		bool inFrustum(const Frustum& frustum);
		ChunkState getState() const;
		
		void setState(ChunkState state);

		inline bool isMeshConstructed() const { return m_mesh->isMeshConstructed(); }
		inline glm::vec3 getPos() const { return m_pos; }
		
		Chunk() = default;
		Chunk(Chunk&&) = default;
		Chunk& operator=(Chunk&&) = default;

	private:
		glm::ivec3 m_pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_center;

		ChunkState m_state = ChunkState::EMPTY;
		std::shared_ptr<Mesh> m_mesh = nullptr;
		std::vector<Voxel> m_voxels;
		std::vector<Vertex> m_vertices;
		std::vector<std::shared_ptr<Chunk>> m_neighbours;

		mutable std::shared_mutex m_mutex;

		enum FaceType : int8_t
		{
			None = -1,
			BackFace = 0,
			FrontFace = 1,
		};

		inline int32_t getId(const glm::vec3& v, const float cSize) { return cSize * (v.y * cSize + v.z) + v.x; }

		bool isOnwardPlane(const Plane& frustum) const;
		void bakeCulled(const std::vector<Voxel>& voxels, const float size);
		void bakeGreedy(const std::vector<Voxel>& voxels, const float size);
		void bakeBinGreedy(const std::vector<Voxel>& voxels, const float size);
		
		void addQuadFace(const glm::vec3& pos, 
			const glm::vec3& u, const glm::vec3& v, 
			const glm::u8vec3& color, 
			const int32_t id);
		void addQuadFace(const glm::vec3& pos, 
			const glm::vec3& u, const glm::vec3& v,
			const int32_t texId, bool isBackFace);

		void defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const int32_t iAxis) const;
#if 0
		void handleAxis(const glm::vec3& pos, TextureFace tFace, const uint32_t iAxis,
			const glm::vec3& u, const glm::vec3& v,
			const int32_t neighbourId,
			Direction dir, bool condition, bool isBackFace);
#endif
	};
}