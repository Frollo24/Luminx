#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "LoadModelLayer.h"

class LoadModelApp : public Luminx::Application
{
public:
	LoadModelApp()
	{
		PushLayer(new LoadModelLayer());
	}
	virtual ~LoadModelApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new LoadModelApp();
}