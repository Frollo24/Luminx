#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "SkyboxLayer.h"

class SimpleFogApp : public Luminx::Application
{
public:
	SimpleFogApp() : Luminx::Application("Skybox Rendering")
	{
		PushLayer(new SkyboxLayer());
	}
	virtual ~SimpleFogApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new SimpleFogApp();
}