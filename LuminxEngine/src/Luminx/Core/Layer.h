#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Events/Event.h"

namespace Luminx
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}

