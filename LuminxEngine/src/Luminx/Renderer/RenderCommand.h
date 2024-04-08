#pragma once

#include "Luminx/Renderer/Pipeline.h"
#include "Luminx/Renderer/VertexArray.h"
#include "Luminx/Renderer/BufferLayout.h"
#include "Luminx/Renderer/Framebuffer.h"

namespace Luminx
{
	class RenderCommand
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass(const Ref<Framebuffer>& framebuffer, const ClearValues& clearValues);
		static void EndRenderPass();
		static void DefaultFramebuffer();

		static void SetViewport(u32 x, u32 y, u32 width, u32 height);
		static void SetDefaultState(const PipelineState& defaultState);
		static void SetDepthState(const PipelineDepthState& depthState);
		static void SetBlendState(const PipelineBlendState& blendState);
		static void SetPolygonState(const PipelinePolygonState& polygonState);

		static void BindVertexArray(const Ref<VertexArray>& vertexArray);
		static void BindUniformBuffer(const Ref<Buffer>& uniformBuffer, u32 index);
		static void DrawVertices(u32 vertexCount, u32 firstIndex = 0);
		static void DrawIndices(u32 indexCount, IndexType type = IndexType::U32);
	};
}

