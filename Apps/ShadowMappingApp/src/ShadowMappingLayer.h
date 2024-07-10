#pragma once
#include <Luminx.h>

class ShadowMappingLayer : public Luminx::Layer
{
public:
	ShadowMappingLayer() : Layer("ShadowMappingLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

