#pragma once
#include <Luminx.h>

class SimpleFogLayer : public Luminx::Layer
{
public:
	SimpleFogLayer() : Layer("SimpleFogLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

