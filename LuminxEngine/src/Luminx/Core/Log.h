#pragma once

#include "Luminx/Core/Base.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Luminx {

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
#define LUM_CORE_TRACE(...)   ::Luminx::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LUM_CORE_INFO(...)    ::Luminx::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LUM_CORE_WARN(...)    ::Luminx::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LUM_CORE_ERROR(...)   ::Luminx::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LUM_CORE_FATAL(...)   ::Luminx::Log::GetCoreLogger()->critical(__VA_ARGS__)

// --- CLIENT LOG MACROS ---
#define LUM_TRACE(...)        ::Luminx::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LUM_INFO(...)         ::Luminx::Log::GetClientLogger()->info(__VA_ARGS__)
#define LUM_WARN(...)         ::Luminx::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LUM_ERROR(...)        ::Luminx::Log::GetClientLogger()->error(__VA_ARGS__)
#define LUM_FATAL(...)        ::Luminx::Log::GetClientLogger()->critical(__VA_ARGS__)