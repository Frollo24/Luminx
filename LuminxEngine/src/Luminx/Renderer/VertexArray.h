#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/Buffer.h"
#include "Luminx/Renderer/BufferLayout.h"

namespace Luminx
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind();
		void Unbind();

		void AddVertexBuffer(const Ref<Buffer>& buffer, const BufferLayout& layout); // Set Vertex Buffer Binding here
		void SetIndexBuffer(const Ref<Buffer>& buffer, const IndexType& size = IndexType::U32); // Set Index Buffer Index Type here (U16 vs U32)

		inline const std::vector<Ref<Buffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		inline const Ref<Buffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		inline u32 GetRendererID() const { return m_RendererID; }
		inline const IndexType& GetIndexType() const { return m_IndexType; }

	private:
		u32 m_RendererID = 0;
		u32 m_VertexAttribPointer = 0;
		IndexType m_IndexType = IndexType::U32;

		std::vector<Ref<Buffer>> m_VertexBuffers{};
		Ref<Buffer> m_IndexBuffer = nullptr;
	};
}

