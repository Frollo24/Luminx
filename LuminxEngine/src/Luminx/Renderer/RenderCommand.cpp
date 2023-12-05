#include "lumpch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Luminx
{
	namespace Utils
	{
		static GLenum DepthComparisonToGLDepthFunc(DepthComparison comp)
		{
			switch (comp)
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

		static GLenum BlendEquationToGLBlendEquation(BlendEquation equation)
		{
			switch (equation)
			{
				case BlendEquation::Add:             return GL_FUNC_ADD;
				case BlendEquation::Subtract:        return GL_FUNC_SUBTRACT;
				case BlendEquation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
				case BlendEquation::Minimum:         return GL_MIN;
				case BlendEquation::Maximum:         return GL_MAX;
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

	void RenderCommand::Init()
	{
		// TODO: include OpenGL debug callback
		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_CULL_FACE);
	}

	void RenderCommand::Shutdown()
	{
		// In OpenGL this does nothing
	}

	void RenderCommand::BeginFrame()
	{
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
		glClearDepthf(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::EndFrame()
	{
		// In OpenGL this does nothing
	}

	void RenderCommand::SetViewport(u32 x, u32 y, u32 width, u32 height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::SetDefaultState()
	{
		// TODO: implement
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
		if (!blendState.BlendEnable)
		{
			glDisable(GL_BLEND);
			return;
		}
		glEnable(GL_BLEND);
		glBlendFuncSeparate(
			Utils::BlendFactorToGLBlendFactor(blendState.SrcColorFactor),
			Utils::BlendFactorToGLBlendFactor(blendState.DstColorFactor),
			Utils::BlendFactorToGLBlendFactor(blendState.SrcAlphaFactor),
			Utils::BlendFactorToGLBlendFactor(blendState.DstAlphaFactor)
		);
		glBlendEquation(Utils::BlendEquationToGLBlendEquation(blendState.Equation));
	}

	void RenderCommand::BindVertexArray(const Ref<VertexArray>& vertexArray)
	{
		glBindVertexArray(vertexArray->GetRendererID());
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