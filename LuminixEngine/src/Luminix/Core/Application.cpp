#include "lumpch.h"
#include "Application.h"

#include "Luminix/Window/WindowSystem.h"

namespace Luminix
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		WindowSystem::Init();
		m_Window = WindowSystem::Create();
		m_Window->SetEventCallback(LUM_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{
		WindowSystem::Destroy(m_Window);
		WindowSystem::Shutdown();
		s_Instance = nullptr;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = Window::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			Time::SetTotalTime(time);
			Time::SetDeltaTime(timestep);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			Input::OnUpdate();
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(LUM_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(LUM_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled()) break;
			(*it)->OnEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		LUM_CORE_INFO("{0}", e);
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Window->SetMinimized(true);
			return false;
		}

		m_Window->SetMinimized(false);
		return false;
	}
}
