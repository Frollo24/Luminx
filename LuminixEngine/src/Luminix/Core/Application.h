#pragma once

#include "Luminix/Core/Base.h"
#include "Luminix/Core/Input.h"
#include "Luminix/Core/LayerStack.h"
#include "Luminix/Core/Time.h"
#include "Luminix/Events/Event.h"
#include "Luminix/Events/WindowEvent.h"
#include "Luminix/Window/Window.h"

namespace Luminix
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		static Application& Instance() { return *s_Instance; }

		void Run();
		void OnEvent(Event& e);

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
