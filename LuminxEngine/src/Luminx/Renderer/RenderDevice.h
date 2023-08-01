#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/Buffer.h"
#include "Luminx/Renderer/Pipeline.h"
#include "Luminx/Renderer/Texture.h"

namespace Luminx
{
	class RenderDevice
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<Pipeline> CreatePipeline(const PipelineState& pipelineState, const Ref<Shader>& shader);
		static void DestroyPipeline(Ref<Pipeline>& pipeline);

		static Ref<Buffer> CreateBuffer(const BufferDescription& bufferDescription);
		static void DestroyBuffer(Ref<Buffer>& buffer);

		static Ref<Texture> CreateTexture(const TextureDescription& textureDescription);
		static void DestroyTexture(Ref<Texture>& texture);
	};
}
