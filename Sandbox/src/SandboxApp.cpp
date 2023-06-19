#define LUMINIX_ENTRY_POINT
#include <Luminix.h>

class Sandbox : public Luminix::Application
{
public:
	Sandbox() = default;
	virtual ~Sandbox() = default;
};

Luminix::Application* Luminix::CreateApplication()
{
	return new Sandbox();
}