#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/VertexArray.h"

namespace Luminx
{
	class RenderEntity
	{
	public:
		RenderEntity(const Ref<VertexArray>& vertexArray, u32 vertexCount, u32 indexCount = 0);
		~RenderEntity();

		inline const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		inline u32 GetVertexCount() const { return m_VertexCount; }
		inline u32 GetIndexCount() const { return m_IndexCount; }

	private:
		Ref<VertexArray> m_VertexArray = nullptr;
		u32 m_VertexCount = 0;
		u32 m_IndexCount = 0;
	};
}

