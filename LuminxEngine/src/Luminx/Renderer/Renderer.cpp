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

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, u32 vertexCount)
	{
		vertexArray->Bind();
		RenderCommand::DrawVertices(vertexCount);
	}

	void Renderer::Submit(const Ref<RenderEntity>& renderEntity)
	{
		RenderCommand::BindVertexArray(renderEntity->GetVertexArray());
		if (renderEntity->GetIndexCount())
			RenderCommand::DrawIndices(renderEntity->GetIndexCount(), renderEntity->GetVertexArray()->GetIndexType());
		else
			RenderCommand::DrawVertices(renderEntity->GetVertexCount());
	}
}