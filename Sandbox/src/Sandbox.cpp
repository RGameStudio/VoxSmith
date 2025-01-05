#include <VoxSmith.hpp>

#define CALC_HEIGHT(width, ratio) (width) / (ratio)

constexpr float g_aspectRatio = 16.0f / 9.0f;
constexpr uint32_t g_tempWidth = 1600;
constexpr uint32_t g_tempHeight = CALC_HEIGHT(g_tempWidth, g_aspectRatio);

constexpr float g_focalLength = 1.0f;

class Sandbox final : public VoxSmith::Application
{
	VoxSmith::Buffer buff;
	VoxSmith::World world;

	std::shared_ptr<VoxSmith::Mesh> mesh1;
	std::shared_ptr<VoxSmith::Mesh> mesh2;
public:
	Sandbox()
		: Application(g_tempWidth, g_tempHeight)
		, world(m_camera->getPos(), 8)
	{
		VoxSmith::ResourceManager::getInstance().getShader(VoxSmith::s_meshTexture)
			.setUniform4m("u_projection", m_camera->getProjection());
		VoxSmith::ResourceManager::getInstance().getShader(VoxSmith::s_chunkOutline)
			.setUniform4m("u_projection", m_camera->getProjection());
		VoxSmith::ResourceManager::getInstance().getShader(VoxSmith::s_meshTexture)
			.setUniform1i("u_textureArray", 0);
		VoxSmith::ResourceManager::getInstance().getTexture(VoxSmith::s_voxelTextureList).use();
	}

	void update(const float dt) override
	{
		updateCamera(dt);
		world.update(m_camera->getPos());
	}

	void updateCamera(const float dt) override
	{
		glm::vec3 vel = glm::vec3(0.0f);
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_W))
		{
			vel += m_camera->getFront();
		}
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_S))
		{
			vel -= m_camera->getFront();
		}
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_D))
		{
			vel += glm::normalize(glm::cross(m_camera->getFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		if (VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_A))
		{
			vel -= glm::normalize(glm::cross(m_camera->getFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
		}

		m_camera->updateCameraPos(vel, dt);
		if (VoxSmith::Mouse::getInstance().moved() &&
			VoxSmith::Keyboard::getInstance().isKeyActive(VOX_KEY_LEFT_SHIFT))
		{
			auto mousePos = VoxSmith::Mouse::getInstance().getMousePos();
			m_camera->updateCameraAngle(mousePos.x, mousePos.y);
		}

		VoxSmith::ResourceManager::getInstance().getShader(VoxSmith::s_meshTexture)
			.setUniform4m("u_view", m_camera->getView());
		VoxSmith::ResourceManager::getInstance().getShader(VoxSmith::s_chunkOutline)
			.setUniform4m("u_view", m_camera->getView());
	}

	void draw(const float dt, const float cframe) override
	{
		world.draw(m_renderer, 
			VoxSmith::ResourceManager::getInstance().getShader(VoxSmith::s_meshTexture), 
			m_camera->getPos(), m_camera->getFrustum(), chunkOutlineStatus());
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
