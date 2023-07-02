#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "SandboxLayer.h"

class Sandbox : public Luminx::Application
{
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
	}
	virtual ~Sandbox() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new Sandbox();
}