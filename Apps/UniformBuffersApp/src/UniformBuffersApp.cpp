#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "UniformBuffersLayer.h"

class UniformBuffersApp : public Luminx::Application
{
public:
	UniformBuffersApp() : Luminx::Application("Uniform Buffers")
	{
		PushLayer(new UniformBuffersLayer());
	}
	virtual ~UniformBuffersApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new UniformBuffersApp();
}