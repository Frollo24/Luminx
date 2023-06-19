#pragma once

#include "Luminix/Core/Base.h"
#include "Luminix/Events/Event.h"

struct GLFWwindow;

namespace Luminix
{
	struct WindowProps
	{
		String Title;
		u32 Width;
		u32 Height;

		WindowProps(const String& title = "Luminix Engine", u32 width = 1600, u32 height = 900)
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
			unsigned int Width = 0;
			unsigned int Height = 0;
			bool Minimized = false;
			bool WasResized = false;
			bool VSync = false;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		GLFWwindow* m_NativeWindow = nullptr;
	};
}

