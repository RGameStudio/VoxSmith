#pragma once

VoxSmith::Application* VoxSmith::createApplication();

int main(int argc, char** argv)
{
	auto app = VoxSmith::createApplication();
	app->run();
	delete app;
}
