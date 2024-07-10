#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "ShadowMappingLayer.h"

class ShadowMappingApp : public Luminx::Application
{
public:
	ShadowMappingApp() : Luminx::Application("Shadow Mapping")
	{
		PushLayer(new ShadowMappingLayer());
	}
	virtual ~ShadowMappingApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new ShadowMappingApp();
}