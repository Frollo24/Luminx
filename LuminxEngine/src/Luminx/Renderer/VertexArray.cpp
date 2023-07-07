#include "lumpch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace Luminx
{
	static GLenum ShaderTypeToOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Float2:   return GL_FLOAT;
			case ShaderDataType::Float3:   return GL_FLOAT;
			case ShaderDataType::Float4:   return GL_FLOAT;
			case ShaderDataType::Mat3:     return GL_FLOAT;
			case ShaderDataType::Mat4:     return GL_FLOAT;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Int2:     return GL_INT;
			case ShaderDataType::Int3:     return GL_INT;
			case ShaderDataType::Int4:     return GL_INT;
			case ShaderDataType::Bool:     return GL_BOOL;
			default:
				return GL_NONE;
		}
	}

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<Buffer>& buffer, const BufferLayout& layout)
	{
		LUM_CORE_VERIFY(buffer->GetDescription().Type == BufferType::Vertex);

		for (const auto& element : layout.GetElements())
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexArrayAttrib(m_RendererID, m_VertexAttribPointer);
					glVertexArrayAttribBinding(m_RendererID, m_VertexAttribPointer, 0);
					glVertexArrayAttribFormat(m_RendererID, m_VertexAttribPointer, element.GetComponentCount(),
						ShaderTypeToOpenGLType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, element.Offset);
					m_VertexAttribPointer++;
					break;
				}

				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexArrayAttrib(m_RendererID, m_VertexAttribPointer);
					glVertexArrayAttribBinding(m_RendererID, m_VertexAttribPointer, 0);
					glVertexArrayAttribIFormat(m_RendererID, m_VertexAttribPointer, element.GetComponentCount(),
						ShaderTypeToOpenGLType(element.Type), element.Offset);

					m_VertexAttribPointer++;
					break;
				}

				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					u8 count = element.GetComponentCount();
					for (u8 i = 0; i < count; i++)
					{
						glEnableVertexArrayAttrib(m_RendererID, m_VertexAttribPointer);
						glVertexArrayAttribBinding(m_RendererID, m_VertexAttribPointer, 0);
						glVertexArrayAttribFormat(m_RendererID, m_VertexAttribPointer, count, ShaderTypeToOpenGLType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE, element.Offset + sizeof(float) * count * i);
						glVertexArrayBindingDivisor(m_RendererID, m_VertexAttribPointer, 1);
						m_VertexAttribPointer++;
					}
					break;
				}

				default:
					break;
			}
		}

		glVertexArrayVertexBuffer(m_RendererID, 0, buffer->GetRendererID(), 0, layout.GetStride());
		m_VertexBuffers.push_back(buffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<Buffer>& buffer, const IndexType& size)
	{
		LUM_CORE_VERIFY(buffer->GetDescription().Type == BufferType::Index);
		glVertexArrayElementBuffer(m_RendererID, buffer->GetRendererID());
		m_IndexType = size;
		m_IndexBuffer = buffer;
	}
}
