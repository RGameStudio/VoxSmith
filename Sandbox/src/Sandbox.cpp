#include <VoxSmith.hpp>

#define CALC_HEIGHT(width, ratio) (width) / (ratio)

constexpr float g_aspectRatio = 16.0f / 9.0f;
constexpr uint32_t g_tempWidth = 1600;
constexpr uint32_t g_tempHeight = CALC_HEIGHT(g_tempWidth, g_aspectRatio);

constexpr float g_focalLength = 1.0f;

class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buff;
	VoxSmith::Shader shader;
	VoxSmith::Chunk chunk1;
	VoxSmith::Chunk chunk2;

	std::shared_ptr<VoxSmith::Mesh> mesh1;
	std::shared_ptr<VoxSmith::Mesh> mesh2;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, shader("shaders/mesh_basic.glsl")
		, chunk1(glm::vec3(0.0f))
		, chunk2(glm::vec3(3.0f, 0.0f, 0.0f))
	{
		chunk1.addNeighbour(VoxSmith::RIGHT, &chunk2);
		chunk2.addNeighbour(VoxSmith::LEFT, &chunk1);

		mesh1 = std::make_shared<VoxSmith::Mesh>();
		mesh2 = std::make_shared<VoxSmith::Mesh>();
		
		chunk1.setMesh(mesh1);
		chunk1.constructMesh();
		chunk2.setMesh(mesh2);
		chunk2.constructMesh();

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
		if (VoxSmith::Mouse::getInstance().moved() &&
			VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_LEFT_SHIFT))
		{
			auto mousePos = VoxSmith::Mouse::getInstance().getMousePos();
			m_camera->updateCameraAngle(mousePos.x, mousePos.y);
		}
		shader.setUniform4m("u_view", m_camera->getView());
	}

	void draw(const float dt, const float cframe) override
	{
		shader.setUniform3fv("u_chunkPos", glm::vec3(0.0f));
		chunk1.draw(m_renderer, shader);
		shader.setUniform3fv("u_chunkPos", glm::vec3(3.0f, 0.0f, 0.0f));
		chunk2.draw(m_renderer, shader);
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
