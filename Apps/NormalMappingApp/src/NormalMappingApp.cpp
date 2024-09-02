#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "NormalMappingLayer.h"

class NormalMappingApp : public Luminx::Application
{
public:
	NormalMappingApp() : Luminx::Application("Normal Mapping")
	{
		PushLayer(new NormalMappingLayer());
	}
	virtual ~NormalMappingApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new NormalMappingApp();
}