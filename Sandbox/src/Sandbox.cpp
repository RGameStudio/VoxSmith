#include <VoxSmith.hpp>

class SandBox final : public VoxSmith::Application
{
public:
	SandBox()
	{

	}
	
	void update() override
	{
		LOG_TRACE("Hi low from the sandbox");
	}
	
	~SandBox() noexcept
	{

	};

	SandBox(const SandBox&) = delete;
	SandBox(SandBox&&) = delete;
	SandBox& operator=(const SandBox&) = delete;
	SandBox* operator=(SandBox&&) = delete;
private:
};

VoxSmith::Application* VoxSmith::createApplication()
{
	return new SandBox();
}
