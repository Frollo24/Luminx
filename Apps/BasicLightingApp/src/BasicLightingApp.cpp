#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "BasicLightingLayer.h"

class BasicLightingApp : public Luminx::Application
{
public:
	BasicLightingApp() : Luminx::Application("Basic Phong Lighting")
	{
		PushLayer(new BasicLightingLayer());
	}
	virtual ~BasicLightingApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new BasicLightingApp();
}