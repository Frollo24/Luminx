#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
	enum BufferType
	{
		Vertex, Index
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
		u32 m_RendererID = 0;
		BufferDescription m_Description{};
	};
}

