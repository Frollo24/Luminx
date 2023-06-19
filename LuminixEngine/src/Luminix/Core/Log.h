#pragma once

#include "Luminix/Core/Base.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Luminix {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// --- CORE LOG MACROS ---
#define LUM_CORE_TRACE(...)   ::Luminix::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LUM_CORE_INFO(...)    ::Luminix::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LUM_CORE_WARN(...)    ::Luminix::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LUM_CORE_ERROR(...)   ::Luminix::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LUM_CORE_FATAL(...)   ::Luminix::Log::GetCoreLogger()->critical(__VA_ARGS__)

// --- CLIENT LOG MACROS ---
#define LUM_TRACE(...)        ::Luminix::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LUM_INFO(...)         ::Luminix::Log::GetClientLogger()->info(__VA_ARGS__)
#define LUM_WARN(...)         ::Luminix::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LUM_ERROR(...)        ::Luminix::Log::GetClientLogger()->error(__VA_ARGS__)
#define LUM_FATAL(...)        ::Luminix::Log::GetClientLogger()->critical(__VA_ARGS__)