#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
	enum class BufferType
	{
		Vertex, Index, Uniform
	};

	struct BufferDescription
	{
		BufferType Type;
		u32 Size;
		void* Data = nullptr;
	};

	class Buffer
	{
	public:
		Buffer(const BufferDescription& desc);
		~Buffer();

		inline u32 GetRendererID() const { return m_RendererID; }
		inline const BufferDescription& GetDescription() const { return m_Description; }

		void SetData(u32 offset, u32 size, const void* data);

	private:
		BufferDescription m_Description{};
		u32 m_RendererID = 0;
	};
}

