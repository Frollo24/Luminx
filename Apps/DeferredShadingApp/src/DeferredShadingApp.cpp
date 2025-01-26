#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "DeferredShadingLayer.h"

class DeferredShadingApp : public Luminx::Application
{
public:
	DeferredShadingApp() : Luminx::Application("Deferred Shading")
	{
		PushLayer(new DeferredShadingLayer());
	}
	virtual ~DeferredShadingApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new DeferredShadingApp();
}