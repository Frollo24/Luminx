#pragma once
#include <Luminx.h>

class LightCastersLayer : public Luminx::Layer
{
public:
	LightCastersLayer() : Layer("LightCastersLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

