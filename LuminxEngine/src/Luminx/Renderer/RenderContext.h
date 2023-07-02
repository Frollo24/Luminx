#pragma once

struct GLFWwindow;

namespace Luminx
{
	class RenderContext
	{
	public:
		RenderContext(GLFWwindow* windowHandle);
		~RenderContext() = default;

		void Init();
		void SetSwapInterval(int interval);
		void SwapBuffers();

	private:
		GLFWwindow* m_WindowHandle = nullptr;
	};
}
