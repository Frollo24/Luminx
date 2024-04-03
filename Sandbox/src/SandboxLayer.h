#pragma once
#include <Luminx.h>

class SandboxLayer : public Luminx::Layer
{
public:
	SandboxLayer() : Layer("SandboxLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

