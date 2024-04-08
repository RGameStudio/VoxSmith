#include <VoxSmith.hpp>

const std::vector<float> data = {
	-1, -1, 0,   0, 0, // left-bot
	 1, -1, 0,   1, 0, // right-bot
	-1,  1, 0,   0, 1, // left-top

	 1, -1, 0,   1, 0, // right-bot
	-1,  1, 0,   0, 1, // left-top
	 1,  1, 0,   1, 1, // right-top
};

constexpr size_t g_tempWidth = 1000.0f;
constexpr size_t g_tempHeight = 1000.0f;

constexpr glm::vec3 g_eyePos = { 0.5f, 0.5f, 1.0f };

float g_focalLength = 3.0f;

class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buffer;
	VoxSmith::ComputeShader cShader;
	VoxSmith::Texture texture;
	VoxSmith::Shader screenQuad;
	VoxSmith::Renderer renderer;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, screenQuad("shaders/simple.glsl")
		, cShader("shaders/compute_test.glsl")
		, texture(g_tempWidth, g_tempHeight)
	{
		VoxSmith::initBuffer(buffer, data);

		cShader.setUniform3fv("g_eyePos", g_eyePos);
		cShader.setUniform1f("g_focalLength", g_focalLength);
	}
	
	float fCounter = 0;
	void update(float dt) override
	{
		if (fCounter > 500)
		{
			LOG_INFO("FPS: {}", (1.0f / dt));
			fCounter = 0;
		}
		else
		{
			fCounter++;
		}
	}

	void draw(float dt, float cframe) override
	{
		cShader.use();
		cShader.launchWorkGroups({ g_tempWidth, g_tempHeight, 1 });

		screenQuad.use();
		texture.use();
		renderer.draw(buffer, screenQuad, texture);
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
