#include "lumpch.h"
#include "Pipeline.h"

#include "RenderCommand.h"

namespace Luminx
{
	Pipeline::Pipeline(const PipelineState& state, const Ref<Shader>& shader)
		: m_State(state), m_Shader(shader)
	{
	}

	void Pipeline::Bind()
	{
		RenderCommand::SetDepthState(m_State.DepthState);
		RenderCommand::SetBlendState(m_State.BlendState);
		m_Shader->Use();
	}
}