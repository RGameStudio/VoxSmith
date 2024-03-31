#include <VoxSmith.hpp>

const std::vector<float> data = {
	0.0f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f
};

constexpr size_t g_tempWidth = 800.0f;
constexpr size_t g_tempHeight = 800.0f;

class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buffer;
	VoxSmith::Shader shader;
	VoxSmith::Renderer renderer;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, shader("shaders/simple.glsl")
	{
		VoxSmith::initBuffer(buffer, data);
	}
	
	void update(float dt) override
	{

	}

	void draw() override
	{
		renderer.draw(buffer, shader);
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
