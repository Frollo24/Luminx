#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "MaterialsPhongLayer.h"

class MaterialsPhongApp : public Luminx::Application
{
public:
	MaterialsPhongApp() : Luminx::Application("Blinn Phong Material Showcase")
	{
		PushLayer(new MaterialsPhongLayer());
	}
	virtual ~MaterialsPhongApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new MaterialsPhongApp();
}