#pragma once

#include "Luminix/Core/Base.h"
#include "Luminix/Window/Window.h"

namespace Luminix
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

