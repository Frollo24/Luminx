#define LUMINIX_ENTRY_POINT
#include <Luminx.h>

#include "TextureLayer.h"

class TextureApp : public Luminx::Application
{
public:
	TextureApp()
	{
		PushLayer(new TextureLayer());
	}
	virtual ~TextureApp() = default;
};

Luminx::Application* Luminx::CreateApplication()
{
	return new TextureApp();
}