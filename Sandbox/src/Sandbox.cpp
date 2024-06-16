#include <VoxSmith.hpp>

#define CALC_HEIGHT(width, ratio) (width) / (ratio)

constexpr float g_aspectRatio = 16.0f / 9.0f;
constexpr uint32_t g_tempWidth = 1600;
constexpr uint32_t g_tempHeight = CALC_HEIGHT(g_tempWidth, g_aspectRatio);

constexpr float g_focalLength = 1.0f;

constexpr glm::vec3 g_eyePos = { 13.0f, 2.0f, 3.0f };
constexpr glm::vec3 g_lookAt = { 0.0f, 0.0f, 0.0f };
constexpr glm::vec3 g_upv = { 0.0f, 1.0f, 0.0f };


class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buffer;
	VoxSmith::ComputeShader cShader;
	VoxSmith::Texture texture;
	VoxSmith::Shader screenQuad;
	VoxSmith::Renderer renderer;
	VoxSmith::RayTracer raytracer;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, screenQuad("shaders/simple.glsl")
		, texture(g_tempWidth, g_tempHeight)
		, raytracer(g_tempWidth, g_tempHeight)
	{
	}
	
	void update(float dt) override
	{

	}

	void draw(float dt, float cframe) override
	{
	}
	
	~Sandbox() noexcept
	{

	}

	Sandbox(const Sandbox&) = delete;
	Sandbox(Sandbox&&) = delete;
	Sandbox& operator=(const Sandbox&) = delete;
	Sandbox* operator=(Sandbox&&) = delete;
};

VoxSmith::Application* VoxSmith::createApplication()
{
	return new Sandbox();
}
