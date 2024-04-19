#include "../Renderer/Renderer.hpp"
#include "../Shader/Shader.hpp"

#include "RayTracer.hpp"

using namespace VoxSmith;

const std::vector<float> g_quadData = {
	-1, -1, 0,   0, 0, // left-bot
	 1, -1, 0,   1, 0, // right-bot
	-1,  1, 0,   0, 1, // left-top

	 1, -1, 0,   1, 0, // right-bot
	-1,  1, 0,   0, 1, // left-top
	 1,  1, 0,   1, 1, // right-top
};

RayTracer::RayTracer(const float width, const float height)
	: m_image(width, height)
{
	initBuffer(m_buffer, g_quadData);
}

void RayTracer::trace(const glm::ivec3& dims, const ComputeShader& cShader) const
{
	cShader.use();
	cShader.launchWorkGroups(dims);
}

void RayTracer::drawImage(const Shader& shader, const Renderer& renderer) const
{
	renderer.draw(m_buffer, shader);
}
