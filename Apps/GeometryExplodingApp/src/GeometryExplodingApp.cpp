#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "GeometryExplodingLayer.h"

class DeferredShadingApp : public Luminx::Application
{
public:
	DeferredShadingApp() : Luminx::Application("Geometry Exploding")
	{
		PushLayer(new GeometryExplodingLayer());
	}
	virtual ~DeferredShadingApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new DeferredShadingApp();
}