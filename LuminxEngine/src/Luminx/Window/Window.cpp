#include "lumpch.h"
#include "Window.h"

#include "Luminx/Events/ApplicationEvent.h"
#include "Luminx/Events/WindowEvent.h"
#include "Luminx/Events/KeyEvent.h"
#include "Luminx/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace Luminx
{
	Window::Window(const WindowProps& props)
	{
		Init(props);
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::OnUpdate()
	{
		if (m_Data.WasResized)
			m_Data.WasResized = false;

		glfwPollEvents();
		m_RenderContext->SwapBuffers();
	}

	void Window::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
		m_RenderContext->SetSwapInterval(enabled ? 1 : 0);
	}

	float Window::GetTime()
	{
		return (float)glfwGetTime();
	}

	void Window::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LUM_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_NativeWindow = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		m_RenderContext = CreateScope<RenderContext>(m_NativeWindow);
		m_RenderContext->Init();

		SetCallbacks();
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_NativeWindow);
	}

	void Window::SetCallbacks()
	{
		glfwSetWindowUserPointer(m_NativeWindow, &m_Data);

		//Set GLFW callbacks
		glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			data.WasResized = true;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_NativeWindow, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event(data.Title);
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			static int repeatCount = 1;
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event((KeyCode)key, 0);
					data.EventCallback(event);
					repeatCount = 1;
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event((KeyCode)key);
					data.EventCallback(event);
					repeatCount = 1;
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event((KeyCode)key, repeatCount);
					data.EventCallback(event);
					repeatCount++;
					break;
				}
				default:
					break;
			}
		});

		glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event((MouseCode)button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event((MouseCode)button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}
}