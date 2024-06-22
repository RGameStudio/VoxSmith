#include <VoxSmith.hpp>

#define CALC_HEIGHT(width, ratio) (width) / (ratio)

constexpr float g_aspectRatio = 16.0f / 9.0f;
constexpr uint32_t g_tempWidth = 1600;
constexpr uint32_t g_tempHeight = CALC_HEIGHT(g_tempWidth, g_aspectRatio);

constexpr float g_focalLength = 1.0f;

std::vector<VoxSmith::Vertex> g_vertices = {
	{glm::vec3(0, 0, 0)},
	{glm::vec3(1, 0, 0)},
	{glm::vec3(0, 1, 0)},

	{glm::vec3(0, 1, 0)},
	{glm::vec3(1, 0, 0)},
	{glm::vec3(1, 1, 0)},
};

class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buff;
	VoxSmith::Renderer renderer;
	VoxSmith::Shader shader;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, shader("shaders/mesh_basic.glsl")
	{
		buff.init(g_vertices);
	}

	void update(const float dt) override
	{
		updateCamera(dt);
	}

	void updateCamera(const float dt) override
	{
		if (VoxSmith::Keyboard::getInstance().getKeyStatus(VOX_KEY_W) == VoxSmith::KeyStatus::PRESS)
		{
			LOG_INFO("PRESS");
		}
		else if (VoxSmith::Keyboard::getInstance().getKeyStatus(VOX_KEY_W) == VoxSmith::KeyStatus::REPEAT)
		{
			LOG_INFO("REPEAT");
		}
		else
		{
			//LOG_INFO("RELEASE");
		}
	}

	void draw(const float dt, const float cframe) override
	{
		renderer.draw(buff, shader);
	}

	~Sandbox() noexcept
	{

	}

	Sandbox(const Sandbox&) = delete;
	Sandbox(Sandbox&&) = delete;
	Sandbox& operator=(const Sandbox&) = delete;
	Sandbox& operator=(Sandbox&&) = delete;
};

VoxSmith::Application* VoxSmith::createApplication()
{
	return new Sandbox();
}
