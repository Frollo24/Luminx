#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Window/Window.h"

namespace Luminx
{
	class WindowSystem
	{
	public:
		static void Init();
		static void Shutdown();

		static Scope<Window> Create(const WindowProps& props = WindowProps());
		static void Destroy(Scope<Window>& window);

	private:
		static bool s_Initialized;
	};
}

