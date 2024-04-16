#pragma once
#include <Luminx.h>

class CameraLayer : public Luminx::Layer
{
public:
	CameraLayer() : Layer("CameraLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

