#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "TriangleLayer.h"

class TriangleApp : public Luminx::Application
{
public:
	TriangleApp() : Luminx::Application("First Triangle")
	{
		PushLayer(new TriangleLayer());
	}
	virtual ~TriangleApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new TriangleApp();
}