#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "SkyboxLayer.h"

class SkyboxApp : public Luminx::Application
{
public:
	SkyboxApp() : Luminx::Application("Skybox Rendering")
	{
		PushLayer(new SkyboxLayer());
	}
	virtual ~SkyboxApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new SkyboxApp();
}