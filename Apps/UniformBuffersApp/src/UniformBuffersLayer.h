#pragma once
#include <Luminx.h>

class UniformBuffersLayer : public Luminx::Layer
{
public:
	UniformBuffersLayer() : Layer("UniformBuffersLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

