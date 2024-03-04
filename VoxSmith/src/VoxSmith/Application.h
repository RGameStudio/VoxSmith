#pragma once

#include "Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	};

	Application* createApplication();
}
