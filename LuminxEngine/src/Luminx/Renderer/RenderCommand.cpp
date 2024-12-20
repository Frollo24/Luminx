#include "lumpch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Luminx
{
	namespace Utils
	{
		static GLenum DepthComparisonToGLDepthFunc(DepthComparison comparison)
		{
			switch (comparison)
			{
				case DepthComparison::None:           return GL_NEVER;
				case DepthComparison::Less:			  return GL_LESS;
				case DepthComparison::Equal:		  return GL_EQUAL;
				case DepthComparison::LessOrEqual:	  return GL_LEQUAL;
				case DepthComparison::Greater:		  return GL_GREATER;
				case DepthComparison::NotEqual:		  return GL_NOTEQUAL;
				case DepthComparison::GreaterOrEqual: return GL_GEQUAL;
				case DepthComparison::Always:         return GL_ALWAYS;
				default:
					return GL_NONE;
			}
		}

		static GLenum BlendFactorToGLBlendFactor(BlendFactor factor)
		{
			switch (factor)
			{
				case BlendFactor::Zero:					 return GL_ZERO;
				case BlendFactor::One:					 return GL_ONE;
				case BlendFactor::SrcColor:				 return GL_SRC_COLOR;
				case BlendFactor::OneMinusSrcColor:		 return GL_ONE_MINUS_SRC_COLOR;
				case BlendFactor::DstColor:				 return GL_DST_COLOR;
				case BlendFactor::OneMinusDstColor:		 return GL_ONE_MINUS_DST_COLOR;
				case BlendFactor::SrcAlpha:				 return GL_SRC_ALPHA;
				case BlendFactor::OneMinusSrcAlpha:		 return GL_ONE_MINUS_SRC_ALPHA;
				case BlendFactor::DstAlpha:				 return GL_DST_ALPHA;
				case BlendFactor::OneMinusDstAlpha:		 return GL_ONE_MINUS_DST_ALPHA;
				case BlendFactor::ConstantColor:		 return GL_CONSTANT_COLOR;
				case BlendFactor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
				case BlendFactor::ConstantAlpha:		 return GL_CONSTANT_ALPHA;
				case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
				default:
					return GL_NONE;
			}
		}

		static GLenum BlendEquationToGLBlendEquation(BlendOperation equation)
		{
			switch (equation)
			{
				case BlendOperation::Add:             return GL_FUNC_ADD;
				case BlendOperation::Subtract:        return GL_FUNC_SUBTRACT;
				case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
				case BlendOperation::Minimum:         return GL_MIN;
				case BlendOperation::Maximum:         return GL_MAX;
				default:
					return GL_NONE;
			}
		}

		static GLenum PolygonRasterModeToGLPolygonMode(PolygonRasterMode polygonMode)
		{
			switch (polygonMode)
			{
				case PolygonRasterMode::Fill:  return GL_FILL;
				case PolygonRasterMode::Line:  return GL_LINE;
				case PolygonRasterMode::Point: return GL_POINT;
				default:
					return GL_NONE;
			}
		}

		static GLenum CullingModeToGLCullFace(CullingMode culling)
		{
			switch (culling)
			{
				case CullingMode::Front:        return GL_FRONT;
				case CullingMode::Back:         return GL_BACK;
				case CullingMode::FrontAndBack: return GL_FRONT_AND_BACK;
				default:
					return GL_NONE;
			}
		}

		static GLenum FrontFaceModeToGLFrontFace(FrontFaceMode frontFace)
		{
			switch (frontFace)
			{
				case FrontFaceMode::CounterClockwise: return GL_CCW;
				case FrontFaceMode::Clockwise:        return GL_CW;
				default:
					return GL_NONE;
			}
		}

		static GLenum IndexTypeToOpenGLType(const IndexType& type)
		{
			switch (type)
			{
				case IndexType::U16: return GL_UNSIGNED_SHORT;
				case IndexType::U32: return GL_UNSIGNED_INT;
				default:
					return GL_NONE;
			}
		}
	}

	static ClearValues s_DefaultClearValues = {};

	void OpenGLDebugCallback(GLenum source, GLenum type, unsigned id, GLenum severity, int length, const char* message, const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         LUM_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       LUM_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_LOW:          LUM_CORE_INFO(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: LUM_CORE_TRACE(message); return;
			default:
				LUM_CORE_ASSERT(false, "Unknown severity level!");
				break;
		}
	}

	void RenderCommand::Init()
	{
#ifdef LUM_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif // LUM_DEBUG

		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		s_DefaultClearValues.Color = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f); // Default Color
	}

	void RenderCommand::Shutdown()
	{
		// In OpenGL this does nothing
	}

	void RenderCommand::BeginFrame()
	{
		glm::vec4 color = s_DefaultClearValues.Color;

		glClearColor(color.r, color.g, color.b, color.a);
		glClearDepthf(s_DefaultClearValues.Depth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::EndFrame()
	{
		// Reset buffer writing to true for any buffer
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	void RenderCommand::BeginRenderPass(const Ref<Framebuffer>& framebuffer, const ClearValues& commonClearValues)
	{
		framebuffer->Bind();

		const FramebufferDescription& description = framebuffer->GetDescription();
		GLuint framebufferID = framebuffer->GetRendererID();
		GLuint drawbuffer = 0;
		int attachmentIndex = 0;
		ClearValues clearValues = {};

		if (commonClearValues.ClearFlags != ClearFlags::None)
			clearValues = commonClearValues;

		for (const AttachmentType& attachment : description.Attachments)
		{
			if (attachment == AttachmentType::None)
				break;

			if (commonClearValues.ClearFlags == ClearFlags::None)
				clearValues = description.ClearValues[attachmentIndex];

			switch (attachment)
			{
				case AttachmentType::Color:
					if ((u8)(clearValues.ClearFlags & ClearFlags::Color))
					{
						std::array<GLfloat, 4> clearColor = { clearValues.Color.r, clearValues.Color.g, clearValues.Color.b, clearValues.Color.a };
						glClearNamedFramebufferfv(framebufferID, GL_COLOR, drawbuffer++, clearColor.data());
					}
					break;
				case AttachmentType::Depth:
					if ((u8)(clearValues.ClearFlags & ClearFlags::Depth))
					{
						GLfloat depth = clearValues.Depth;
						glClearNamedFramebufferfv(framebufferID, GL_DEPTH, 0, &depth);
					}
					break;
				case AttachmentType::Stencil:
					if ((u8)(clearValues.ClearFlags & ClearFlags::Stencil))
					{
						GLint stencil = clearValues.Stencil;
						glClearNamedFramebufferiv(framebufferID, GL_STENCIL, 0, &stencil);
					}
					break;
				case AttachmentType::DepthStencil:
					if (clearValues.ClearFlags == ClearFlags::DepthStencil)
					{
						GLfloat depth = clearValues.Depth;
						GLint stencil = clearValues.Stencil;
						glClearNamedFramebufferfi(framebufferID, GL_DEPTH_STENCIL, 0, depth, stencil);
					}
					break;
				default:
					break;
			}

			attachmentIndex++;
		}
	}

	void RenderCommand::EndRenderPass()
	{
		// At the moment this does nothing
	}

	void RenderCommand::DefaultFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderCommand::DefaultClearValues(const ClearValues& clearValues)
	{
		s_DefaultClearValues = clearValues;
	}

	void RenderCommand::SetViewport(u32 x, u32 y, u32 width, u32 height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::SetDefaultState(const PipelineState& defaultState)
	{
		RenderCommand::SetDepthState(defaultState.DepthState);
		RenderCommand::SetBlendState(defaultState.BlendState);
		RenderCommand::SetPolygonState(defaultState.PolygonState);
	}

	void RenderCommand::SetDepthState(const PipelineDepthState& depthState)
	{
		if (!depthState.DepthTest)
		{
			glDisable(GL_DEPTH_TEST);
			return;
		}
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(Utils::DepthComparisonToGLDepthFunc(depthState.DepthFunc));
		glDepthRange(depthState.DepthRange.MinDepth, depthState.DepthRange.MaxDepth);
		glDepthMask(depthState.DepthWrite ? GL_TRUE : GL_FALSE);
	}

	void RenderCommand::SetBlendState(const PipelineBlendState& blendState)
	{
		for (int idx = 0; idx < blendState.BlendAttachments.size(); idx++)
		{
			const BlendAttachment& blendAttachment = blendState.BlendAttachments[idx];
			if (!blendAttachment.BlendEnable)
			{
				glDisablei(GL_BLEND, idx);
				continue;
			}
			glEnablei(GL_BLEND, idx);

			glBlendFuncSeparatei(idx,
				Utils::BlendFactorToGLBlendFactor(blendAttachment.ColorEquation.SrcFactor),
				Utils::BlendFactorToGLBlendFactor(blendAttachment.ColorEquation.DstFactor),
				Utils::BlendFactorToGLBlendFactor(blendAttachment.AlphaEquation.SrcFactor),
				Utils::BlendFactorToGLBlendFactor(blendAttachment.AlphaEquation.DstFactor)
			);

			glBlendEquationSeparatei(idx,
				Utils::BlendEquationToGLBlendEquation(blendAttachment.ColorEquation.Operation),
				Utils::BlendEquationToGLBlendEquation(blendAttachment.AlphaEquation.Operation)
			);

			glColorMaski(idx,
				GLboolean(blendAttachment.ColorWriteMask & ColorWriteMask::ColorWriteMaskR),
				GLboolean(blendAttachment.ColorWriteMask & ColorWriteMask::ColorWriteMaskG),
				GLboolean(blendAttachment.ColorWriteMask & ColorWriteMask::ColorWriteMaskB),
				GLboolean(blendAttachment.ColorWriteMask & ColorWriteMask::ColorWriteMaskA)
			);
		}

		glBlendColor(blendState.ConstantColor.R, blendState.ConstantColor.G, blendState.ConstantColor.B, blendState.ConstantColor.A);
	}

	void RenderCommand::SetPolygonState(const PipelinePolygonState& polygonState)
	{
		if (!polygonState.CullEnable)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
		glFrontFace(Utils::FrontFaceModeToGLFrontFace(polygonState.FrontFace));
		glCullFace(Utils::CullingModeToGLCullFace(polygonState.CullMode));
		glPolygonMode(GL_FRONT_AND_BACK, Utils::PolygonRasterModeToGLPolygonMode(polygonState.PolygonMode));
	}

	void RenderCommand::BindVertexArray(const Ref<VertexArray>& vertexArray)
	{
		glBindVertexArray(vertexArray->GetRendererID());
	}

	void RenderCommand::BindUniformBuffer(const Ref<Buffer>& uniformBuffer, u32 index)
	{
		LUM_CORE_ASSERT(uniformBuffer->GetDescription().Type == BufferType::Uniform,
			"Trying to bind a buffer which is not a Uniform Buffer Object!");
		glBindBufferBase(GL_UNIFORM_BUFFER, index, uniformBuffer->GetRendererID());
	}

	void RenderCommand::DrawVertices(u32 vertexCount, u32 firstVertex)
	{
		glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
	}

	void RenderCommand::DrawIndices(u32 indexCount, IndexType type)
	{
		glDrawElements(GL_TRIANGLES, indexCount, Utils::IndexTypeToOpenGLType(type), nullptr);
	}
}