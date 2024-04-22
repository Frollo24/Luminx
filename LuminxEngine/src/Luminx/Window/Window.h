#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Events/Event.h"
#include "Luminx/Renderer/RenderContext.h"

struct GLFWwindow;

namespace Luminx
{
	struct WindowProps
	{
		String Title;
		u32 Width;
		u32 Height;

		WindowProps(const String& title = "Luminx Engine", u32 width = 1600, u32 height = 900)
			: Title(title), Width(width), Height(height) {}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props = WindowProps());
		~Window();

		void OnUpdate();
		unsigned int GetWidth() const { return m_Data.Width; }
		unsigned int GetHeight() const { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		inline void SetMinimized(bool minimized) { m_Data.Minimized = minimized; }
		inline bool IsMinimized() const { return m_Data.Minimized; }
		void SetVSync(bool enabled);
		inline bool IsVSync() const { return m_Data.VSync; }

		inline GLFWwindow* GetNativeWindow() const { return m_NativeWindow; }
		static float GetTime();

	private:
		void Init(const WindowProps& props);
		void Shutdown();
		void SetCallbacks();

	private:
		struct WindowData
		{
			std::string Title;
			u32 Width = 0;
			u32 Height = 0;
			bool Minimized = false;
			bool WasResized = false;
			bool VSync = false;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		GLFWwindow* m_NativeWindow = nullptr;
		Scope<RenderContext> m_RenderContext = nullptr;
	};
}

