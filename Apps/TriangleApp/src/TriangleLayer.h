#pragma once
#include <Luminx.h>

class TriangleLayer : public Luminx::Layer
{
public:
	TriangleLayer() : Layer("TriangleLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
};

