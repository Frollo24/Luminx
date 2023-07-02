#include "lumpch.h"
#include "BufferLayout.h"

namespace Luminx
{
	void BufferLayout::CalculateStrideAndOffset()
	{
		u32 offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements) {
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}
}