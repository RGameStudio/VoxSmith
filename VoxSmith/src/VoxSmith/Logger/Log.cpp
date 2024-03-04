#include "spdlog/sinks/stdout_color_sinks.h"

#include "Log.hpp"

using namespace VoxSmith;

std::shared_ptr<spdlog::logger> VoxSmith::Log::s_coreLogger;
std::shared_ptr<spdlog::logger> VoxSmith::Log::s_clientLogger;

void Log::init()
{
	spdlog::set_pattern("%^ [%T]::%n::%v%$");

	s_coreLogger = spdlog::stdout_color_mt("VOXSMITH_LOGGER");
	s_coreLogger->set_level(spdlog::level::trace);

	s_clientLogger = spdlog::stdout_color_mt("CLIENT_LOGGER");
	s_clientLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Log::getCoreLogger()
{
	return s_coreLogger;
}

std::shared_ptr<spdlog::logger>& Log::getAppLogger()
{
	return s_clientLogger;
}
