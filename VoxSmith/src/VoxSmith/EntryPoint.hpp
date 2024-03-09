#pragma once

VoxSmith::Application* VoxSmith::createApplication();

int main(int argc, char** argv)
{
	VoxSmith::Log::init();

	LOG_CORE_INFO("Oh you touch my talala la la la");

	auto app = VoxSmith::createApplication();
	app->run();
	delete app;
}
