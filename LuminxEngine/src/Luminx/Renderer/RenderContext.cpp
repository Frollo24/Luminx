#include "lumpch.h"
#include "RenderContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Luminx
{
	RenderContext::RenderContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		LUM_CORE_ASSERT((void*)m_WindowHandle, "Window handle is null!");
		glfwMakeContextCurrent(m_WindowHandle);
	}

	void RenderContext::Init()
	{
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LUM_CORE_ASSERT(status, "Failed to initialize Glad!");

		LUM_CORE_INFO("OpenGL Info:");
		LUM_CORE_INFO("  Vendor: {0}", std::string_view((const char*)glGetString(GL_VENDOR)));
		LUM_CORE_INFO("  Renderer: {0}", std::string_view((const char*)glGetString(GL_RENDERER)));
		LUM_CORE_INFO("  Version: {0}", std::string_view((const char*)glGetString(GL_VERSION)));

		LUM_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Luminx requires at least OpenGL version 4.5!");
	}

	void RenderContext::SetSwapInterval(int interval)
	{
		glfwSwapInterval(interval);
	}

	void RenderContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}