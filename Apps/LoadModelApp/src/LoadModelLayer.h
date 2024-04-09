#pragma once
#include <Luminx.h>

class LoadModelLayer : public Luminx::Layer
{
public:
	LoadModelLayer() : Layer("LoadModelLayer") {}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
};

