#pragma once
#include <Luminx.h>

class DeferredShadingLayer : public Luminx::Layer
{
public:
	DeferredShadingLayer() : Layer("DeferredShadingLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

