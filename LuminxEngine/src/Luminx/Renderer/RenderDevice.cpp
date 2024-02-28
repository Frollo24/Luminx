#include "lumpch.h"
#include "RenderDevice.h"

namespace Luminx
{
	Ref<Pipeline> RenderDevice::CreatePipeline(const PipelineState& pipelineState, const Ref<Shader>& shader)
	{
		return CreateRef<Pipeline>(pipelineState, shader);
	}

	void RenderDevice::DestroyPipeline(Ref<Pipeline>& pipeline)
	{
		pipeline = nullptr;
	}

	Ref<Buffer> RenderDevice::CreateBuffer(const BufferDescription& bufferDescription)
	{
		return CreateRef<Buffer>(bufferDescription);
	}

	void RenderDevice::DestroyBuffer(Ref<Buffer>& buffer)
	{
		buffer = nullptr;
	}

	Ref<Texture> RenderDevice::CreateTexture(const TextureDescription& textureDescription)
	{
		return CreateRef<Texture>(textureDescription);
	}

	void RenderDevice::DestroyTexture(Ref<Texture>& texture)
	{
		texture = nullptr;
	}

	Ref<Framebuffer> RenderDevice::CreateFramebuffer(const FramebufferDescription& framebufferDescription)
	{
		return CreateRef<Framebuffer>(framebufferDescription);
	}

	void RenderDevice::DestroyFramebuffer(Ref<Framebuffer>& framebuffer)
	{
		framebuffer = nullptr;
	}
}
