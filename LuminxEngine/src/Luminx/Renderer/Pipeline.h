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
		
		inline const PipelineState& GetPipelineState() const { return m_State; }
		inline const Ref<Shader>& GetShader() const { return m_Shader; }

	private:
		PipelineState m_State{};
		Ref<Shader> m_Shader = nullptr;
	};
}

