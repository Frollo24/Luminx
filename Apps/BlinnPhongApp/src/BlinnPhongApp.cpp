#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "BlinnPhongLayer.h"

class BlinnPhongApp : public Luminx::Application
{
public:
	BlinnPhongApp() : Luminx::Application("Blinn Phong Lighting")
	{
		PushLayer(new BlinnPhongLayer());
	}
	virtual ~BlinnPhongApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new BlinnPhongApp();
}