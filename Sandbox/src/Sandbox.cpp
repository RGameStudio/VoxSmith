#include <VoxSmith.hpp>

constexpr size_t g_tempWidth = 1600.0f;
constexpr size_t g_tempHeight = 900.0f;

constexpr glm::vec3 g_eyePos = { 0.0f, 0.0f, 20.0f };

constexpr float g_focalLength = 1.0f + g_eyePos.z;

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
	}
	
	void update(float dt) override
	{

	}

	void draw(float dt, float cframe) override
	{
		raytracer.trace({ g_tempWidth / 10, g_tempHeight / 10, 1 }, cShader);
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
