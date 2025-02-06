#pragma once
#include <Luminx.h>

class GeometryExplodingLayer : public Luminx::Layer
{
public:
	GeometryExplodingLayer() : Layer("GeometryExplodingLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

