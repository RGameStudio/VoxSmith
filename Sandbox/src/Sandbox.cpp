#include <VoxSmith.hpp>

class SandBox : public VoxSmith::Application
{
public:
	SandBox() = default;
	~SandBox() = default;

private:
};

VoxSmith::Application* VoxSmith::createApplication()
{
	return new SandBox();
}
