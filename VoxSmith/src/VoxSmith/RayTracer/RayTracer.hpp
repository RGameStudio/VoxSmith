#pragma once

#include <glm/glm.hpp>

#include "../Texture/Texture.hpp"

#include "../Core.hpp"

namespace VoxSmith
{
	class ComputeShader;
	class Shader;
	class Renderer;

	struct Buffer;

	class VOX_SMITH_API RayTracer final
	{
	public:
		RayTracer(const float width, const float height);
		~RayTracer() = default;

		void trace(const glm::ivec3& dims, const ComputeShader& cShader) const;
		void drawImage(const Shader& shader, const Renderer& renderer) const;

		inline const Texture& getImage() const { return m_image; }

	private:
		Texture m_image;
		
		Buffer m_buffer;
 	};
}
