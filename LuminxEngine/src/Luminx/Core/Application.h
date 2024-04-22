#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Core/Input.h"
#include "Luminx/Core/LayerStack.h"
#include "Luminx/Core/Time.h"
#include "Luminx/Events/Event.h"
#include "Luminx/Events/WindowEvent.h"
#include "Luminx/Window/Window.h"

namespace Luminx
{
	class Application
	{
	public:
		Application(const std::string& appName = "");
		virtual ~Application();
		static Application& Instance() { return *s_Instance; }

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		static Application* s_Instance;
		bool m_Running = true;
		float m_LastFrameTime = 0.0f;

		Scope<Window> m_Window;
		LayerStack m_LayerStack;
	};

	// To be defined by client
	Application* CreateApplication();
}
