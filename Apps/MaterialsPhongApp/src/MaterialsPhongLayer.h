#pragma once
#include <Luminx.h>

class MaterialsPhongLayer : public Luminx::Layer
{
public:
	MaterialsPhongLayer() : Layer("MaterialsPhongLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Luminx::Event& e) override;

	bool OnWindowResize(Luminx::WindowResizeEvent& e);
};

