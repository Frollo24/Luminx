#include "lumpch.h"
#include "Renderer.h"

#include "RenderCommand.h"

namespace Luminx
{
	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::Shutdown()
	{
		RenderCommand::Shutdown();
	}

	void Renderer::BeginFrame()
	{
		RenderCommand::BeginFrame();
	}

	void Renderer::EndFrame()
	{
		RenderCommand::EndFrame();
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawVertices(3); // TODO: get vertex count
	}
}