#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include "Buffer.hpp"

#include "../Core.hpp"

namespace VoxSmith
{
	class Shader;
	class Texture;
	struct Vertex;

	class VOX_SMITH_API Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		void switchCulling() const;

		void draw(const Buffer& buffer, const Shader& shader, const int32_t count = 6) const;
		void draw(const Buffer& buffer, const Shader& shader, const Texture& texture) const;
		void drawOutline(const Shader& shader, const glm::vec3& pos) const;

	private:
		friend class Application;

		bool m_showEdges = false;
		bool m_cullingStatus = true;

		Buffer m_outlineBuff;
	};
}
