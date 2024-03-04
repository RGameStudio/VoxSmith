#pragma once

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;
	};

	Application* createApplication();
}
