#pragma once
#include <Luminx.h>

class NormalMappingLayer : public Luminx::Layer
{
public:
	NormalMappingLayer() : Layer("NormalMappingLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

