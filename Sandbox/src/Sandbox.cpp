#include <VoxSmith.hpp>

class SandBox : public VoxSmith::Application
{
public:
	SandBox() = default;
	~SandBox() = default;

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
