#include "lumpch.h"
#include "RenderEntity.h"

namespace Luminx
{
	RenderEntity::RenderEntity(const Ref<VertexArray>& vertexArray, u32 vertexCount, u32 indexCount)
		: m_VertexArray(vertexArray), m_VertexCount(vertexCount), m_IndexCount(indexCount)
	{
	}

	RenderEntity::~RenderEntity()
	{
		m_VertexArray = nullptr;
	}
}
