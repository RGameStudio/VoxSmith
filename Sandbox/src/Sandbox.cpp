#include <VoxSmith.hpp>

#define CALC_HEIGHT(width, ratio) (width) / (ratio)

constexpr float g_aspectRatio = 16.0f / 9.0f;
constexpr uint32_t g_tempWidth = 1600;
constexpr uint32_t g_tempHeight = CALC_HEIGHT(g_tempWidth, g_aspectRatio);

constexpr glm::vec3 g_eyePos = { 0.0f, 0.0f, 0.0f };

constexpr float g_focalLength = 1.0f;

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
		, cShader("shaders/compute_test.glsl")
		, texture(g_tempWidth, g_tempHeight)
		, raytracer(g_tempWidth, g_tempHeight)
	{
		cShader.setUniform3fv("g_eyePos", g_eyePos);
		cShader.setUniform1f("g_focalLength", g_focalLength);
		cShader.setUniform1f("g_screenWidth", static_cast<float>(g_tempWidth));
		cShader.setUniform1f("g_screenHeight", static_cast<float>(g_tempHeight));
	}
	
	void update(float dt) override
	{

	}

	void draw(float dt, float cframe) override
	{
		raytracer.trace({ g_tempWidth / 1, g_tempHeight / 1, 1 }, cShader);
		raytracer.drawImage(screenQuad, renderer);
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
