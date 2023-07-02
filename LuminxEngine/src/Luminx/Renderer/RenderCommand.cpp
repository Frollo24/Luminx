#include "lumpch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Luminx
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

	void RenderCommand::Init()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);
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

	void RenderCommand::SetDepthState(const PipelineDepthState& depthState)
	{
		depthState.DepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		glDepthFunc(DepthComparisonToGLDepthFunc(depthState.DepthFunc));
		glDepthRange(depthState.DepthRange.MinDepth, depthState.DepthRange.MaxDepth);
		glDepthMask(depthState.DepthWrite ? GL_TRUE : GL_FALSE);
	}

	void RenderCommand::DrawVertices(u32 vertexCount, u32 firstVertex)
	{
		glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
	}

	void RenderCommand::DrawIndices(u32 indexCount, IndexType type)
	{
		glDrawElements(GL_TRIANGLES, indexCount, IndexTypeToOpenGLType(type), nullptr);
	}
}