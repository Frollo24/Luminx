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
}
