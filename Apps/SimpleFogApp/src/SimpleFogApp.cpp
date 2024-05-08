#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "SimpleFogLayer.h"

class SimpleFogApp : public Luminx::Application
{
public:
	SimpleFogApp() : Luminx::Application("Simple Fog Rendering")
	{
		PushLayer(new SimpleFogLayer());
	}
	virtual ~SimpleFogApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new SimpleFogApp();
}