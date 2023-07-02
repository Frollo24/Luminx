#pragma once
#include <Luminx.h>

class SandboxLayer : public Luminx::Layer
{
public:
	SandboxLayer() : Layer("SandboxLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
};

