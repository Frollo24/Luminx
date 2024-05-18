#pragma once
#include <Luminx.h>

class SkyboxLayer : public Luminx::Layer
{
public:
	SkyboxLayer() : Layer("SkyboxLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

