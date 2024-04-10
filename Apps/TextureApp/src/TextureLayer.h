#pragma once
#include <Luminx.h>

class TextureLayer : public Luminx::Layer
{
public:
	TextureLayer() : Layer("TextureLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnKeyPressedEvent(Luminx::KeyPressedEvent& e);
};

