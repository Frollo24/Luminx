#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

class Sandbox : public Luminx::Application
{
public:
	Sandbox() = default;
	virtual ~Sandbox() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new Sandbox();
}