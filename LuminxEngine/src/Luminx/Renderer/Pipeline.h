#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/Shader.h"
#include "Luminx/Renderer/PipelineState.h"

namespace Luminx
{
	// TODO: expand
	struct PipelineState
	{
		PipelineDepthState DepthState{};
		PipelineBlendState BlendState{};
		PipelinePolygonState PolygonState{};
	};

	class Pipeline
	{
	public:
		Pipeline(const PipelineState& state, const Ref<Shader>& shader);
		~Pipeline() = default;

		void Bind();

	private:
		PipelineState m_State{};
		Ref<Shader> m_Shader = nullptr;
	};
}

