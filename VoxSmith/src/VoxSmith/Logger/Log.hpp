#pragma once

#include <memory>
#include <spdlog/spdlog.h>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Log
	{
	public:
		static void init();

		static std::shared_ptr<spdlog::logger>& getCoreLogger();
		static std::shared_ptr<spdlog::logger>& getAppLogger();

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

#define LOG_CORE_INFO(...) VoxSmith::Log::getCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_TRACE(...) VoxSmith::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_WARN(...) VoxSmith::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...) VoxSmith::Log::getCoreLogger()->error(__VA_ARGS__)

#define LOG_INFO(...) VoxSmith::Log::getCoreLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) VoxSmith::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LOG_WARN(...) VoxSmith::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) VoxSmith::Log::getCoreLogger()->error(__VA_ARGS__)
