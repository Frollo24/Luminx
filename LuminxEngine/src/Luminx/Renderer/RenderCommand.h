#pragma once

#include "Luminx/Renderer/PipelineState.h"
#include "Luminx/Renderer/VertexArray.h"
#include "Luminx/Renderer/BufferLayout.h"

namespace Luminx
{
	class RenderCommand
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void SetViewport(u32 x, u32 y, u32 width, u32 height);
		static void SetDefaultState();  // TODO: implement
		static void SetDepthState(const PipelineDepthState& depthState);
		static void SetBlendState(const PipelineBlendState& blendState);
		static void SetPolygonState(const PipelinePolygonState& polygonState);

		static void BindVertexArray(const Ref<VertexArray>& vertexArray);
		static void BindUniformBuffer(const Ref<Buffer>& uniformBuffer, u32 index);
		static void DrawVertices(u32 vertexCount, u32 firstIndex = 0);
		static void DrawIndices(u32 indexCount, IndexType type = IndexType::U32);
	};
}

