#pragma once
#include "Luminx.h"

extern Luminx::Application* Luminx::CreateApplication(); // To be defined by client

namespace Luminx
{
	int Main(int argc, char** argv)
	{
		Luminx::Log::Init();
		LUM_CORE_WARN("Initialized Log!");

		auto app = Luminx::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}
}

#ifdef LUM_DIST

#ifdef LUM_PLATFORM_WINDOWS

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ LPSTR cmdline, _In_ int cmdshow) {
	return Luminx::Main(__argc, __argv);
}

#else

int main(int argc, char** argv) {
	return Luminx::Main(argc, argv);
}

#endif // LUM_PLATFORM_WINDOWS

#else

int main(int argc, char** argv) {
	return Luminx::Main(argc, argv);
}

#endif // LUM_DIST