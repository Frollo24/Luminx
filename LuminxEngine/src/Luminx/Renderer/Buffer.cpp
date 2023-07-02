#include "lumpch.h"
#include "Buffer.h"

#include <glad/glad.h>

namespace Luminx
{
	Buffer::Buffer(const BufferDescription& desc)
		: m_Description(desc)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, desc.Size, desc.Data, GL_STATIC_DRAW);
	}

	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void Buffer::SetData(u32 offset, u32 size, const void* data)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}
}