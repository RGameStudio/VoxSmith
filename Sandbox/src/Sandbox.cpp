#include <VoxSmith.hpp>

const std::vector<float> data = {
	0.0f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f
};

class SandBox final : public VoxSmith::Application
{
	VoxSmith::Buffer buffer;
	VoxSmith::Shader shader;
	VoxSmith::Renderer renderer;
public:
	SandBox()
		: shader("shaders/simple.glsl")
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
	
	~SandBox() noexcept
	{

	};

	SandBox(const SandBox&) = delete;
	SandBox(SandBox&&) = delete;
	SandBox& operator=(const SandBox&) = delete;
	SandBox* operator=(SandBox&&) = delete;
};

VoxSmith::Application* VoxSmith::createApplication()
{
	return new SandBox();
}
