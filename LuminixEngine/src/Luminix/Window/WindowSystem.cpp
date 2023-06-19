#include "lumpch.h"
#include "WindowSystem.h"

#include <GLFW/glfw3.h>

namespace Luminix
{
	bool WindowSystem::s_Initialized = false;

	void WindowSystem::Init()
	{
		if (!s_Initialized) {
			int success = glfwInit();
			LUM_CORE_ASSERT(success, "Could not initialize GLFW!");

			s_Initialized = true;
			GLFWerrorfun GLFWErrorCallback = [](int error, const char* description) {
				LUM_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
			};

			glfwSetErrorCallback(GLFWErrorCallback);
		}
	}

	void WindowSystem::Shutdown()
	{
		glfwTerminate();

		s_Initialized = false;
	}

	Scope<Window> WindowSystem::Create(const WindowProps& props)
	{
		LUM_CORE_ASSERT(s_Initialized, "Cannot create a window without initializing the Window System!");
		return CreateScope<Window>(props);
	}

	void WindowSystem::Destroy(Scope<Window>& window)
	{
		window = nullptr;
	}
}