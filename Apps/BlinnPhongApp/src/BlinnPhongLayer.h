#pragma once
#include <Luminx.h>

class BlinnPhongLayer : public Luminx::Layer
{
public:
	BlinnPhongLayer() : Layer("BlinnPhongLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

