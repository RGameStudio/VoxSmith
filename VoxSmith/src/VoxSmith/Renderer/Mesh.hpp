#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "../Chunk/Voxel.hpp"

#include "Buffer.hpp"
#include "../Core.hpp"

namespace VoxSmith
{
	class Renderer;
	class Shader;
	class Buffer;

	struct VOX_SMITH_API Vertex final
	{
		// int16_t data;
		glm::vec3 pos;
	};

	class VOX_SMITH_API Mesh final
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void loadToBuffer();
		void bakeStupid(const std::vector<Voxel>& voxels, const float size);
		void bakeCulled(const std::vector<Voxel>& voxels, const float size);
		void bakeGreedy(const std::vector<Voxel>& voxels, const float size);
		void bakeBinGreedy(const std::vector<Voxel>& voxels, const float size);

		void draw(const std::shared_ptr<Renderer>& renderer, const Shader& shader) const;

	private:
		enum FaceType : int8_t
		{
			None = -1,
			BackFace = 0,
			FrontFace = 1,
		};

		void addQuadFace(const glm::vec3& pos, const glm::vec3& u, const glm::vec3& v);
		void defineUV(glm::vec3& u, glm::vec3& v, const glm::vec2& size, const FaceType face, const int32_t iAxis) const;

		int32_t getId(const glm::vec3& v, const float cSize);

		std::vector<Vertex> m_vertices;

		Buffer m_buffer;
	};
}