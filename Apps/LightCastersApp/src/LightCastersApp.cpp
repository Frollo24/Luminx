#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "LightCastersLayer.h"

class LightCastersApp : public Luminx::Application
{
public:
	LightCastersApp() : Luminx::Application("Light Casters Types")
	{
		PushLayer(new LightCastersLayer());
	}
	virtual ~LightCastersApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new LightCastersApp();
}