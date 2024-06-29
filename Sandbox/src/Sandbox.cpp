#include <VoxSmith.hpp>

#define CALC_HEIGHT(width, ratio) (width) / (ratio)

constexpr float g_aspectRatio = 16.0f / 9.0f;
constexpr uint32_t g_tempWidth = 1600;
constexpr uint32_t g_tempHeight = CALC_HEIGHT(g_tempWidth, g_aspectRatio);

constexpr float g_focalLength = 1.0f;

class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buff;
	VoxSmith::Renderer renderer;
	VoxSmith::Shader shader;
	VoxSmith::Chunk chunk;
	std::shared_ptr<VoxSmith::Mesh> mesh;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, shader("shaders/mesh_basic.glsl")
		, chunk(glm::vec3(0.0f))
	{
		mesh = std::make_shared<VoxSmith::Mesh>();
		chunk.setMesh(mesh);
		chunk.constructMesh();
		shader.setUniform3fv("u_chunkPos", glm::vec3(0.0f));
 		shader.setUniform4m("u_projection", m_camera->getProjection());
		shader.setUniform4m("u_view", m_camera->getView());
	}

	void update(const float dt) override
	{
		updateCamera(dt);
	}

	void updateCamera(const float dt) override
	{
		glm::vec3 vel = glm::vec3(0.0f);
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_W))
		{
			vel += m_camera->getDir();
		}
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_S))
		{
			vel -= m_camera->getDir();
		}
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_D))
		{
			vel += glm::normalize(glm::cross(m_camera->getDir(), glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_A))
		{
			vel -= glm::normalize(glm::cross(m_camera->getDir(), glm::vec3(0.0f, 1.0f, 0.0f)));
		}

		m_camera->updateCameraPos(vel, dt);
		if (VoxSmith::Mouse::getInstance().moved())
		{
			auto mousePos = VoxSmith::Mouse::getInstance().getMousePos();
			m_camera->updateCameraAngle(mousePos.x, mousePos.y);
		}
		shader.setUniform4m("u_view", m_camera->getView());
	}

	void draw(const float dt, const float cframe) override
	{
		chunk.draw(renderer, shader);
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
