#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "CameraLayer.h"

class CameraApp : public Luminx::Application
{
public:
	CameraApp() : Luminx::Application("Basic Camera")
	{
		PushLayer(new CameraLayer());
	}
	virtual ~CameraApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new CameraApp();
}