#include "lumpch.h"
#include "Framebuffer.h"
#include "RenderDevice.h"

#include <glad/glad.h>

namespace Luminx
{
	static GLenum AttachmentTypeToOpenGLAttachment(const AttachmentType& attachment)
	{
		switch (attachment)
		{
			case AttachmentType::None:         return GL_NONE;
			case AttachmentType::Color:        return GL_COLOR_ATTACHMENT0;
			case AttachmentType::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
			case AttachmentType::Depth:        return GL_DEPTH_ATTACHMENT;
			case AttachmentType::Stencil:      return GL_STENCIL;
			default:
				return GL_NONE;
		}
	}

	Framebuffer::Framebuffer(const FramebufferDescription& desc)
		: m_Description(desc)
	{
		glCreateFramebuffers(1, &m_RendererID);
		GLsizei drawbufferCount = 0;
		for (int i = 0; i < m_Description.RenderTargets.size(); i++)
		{
			const Ref<Texture>& renderTarget = m_Description.RenderTargets[i];
			const AttachmentType& attachment = m_Description.Attachments[i];
			if (!renderTarget)
				break;

			GLenum openglAttachment = AttachmentTypeToOpenGLAttachment(attachment);
			if (openglAttachment == GL_COLOR_ATTACHMENT0)
				openglAttachment += drawbufferCount;

			glNamedFramebufferTexture(m_RendererID, openglAttachment, renderTarget->GetRendererID(), 0);
			m_RenderTargetCount++;

			if (attachment == AttachmentType::Color)
				drawbufferCount++;
		}

		if (drawbufferCount)
		{
			std::array<GLenum, MAX_COLOR_ATTACHMENTS> drawbuffers = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
			};
			glNamedFramebufferDrawBuffers(m_RendererID, drawbufferCount, drawbuffers.data());
		}
		else
		{
			glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);
		}

		LUM_CORE_ASSERT(glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(u32 width, u32 height)
	{
		m_Description.Width = width;
		m_Description.Height = height;

		std::vector<TextureDescription> resizedDescs;
		resizedDescs.reserve(m_RenderTargetCount);

		for (auto& renderTarget : m_Description.RenderTargets)
		{
			if (!renderTarget)
				break;

			TextureDescription newTexDesc{};
			newTexDesc.ImageExtent = { width, height, 1 };
			newTexDesc.ImageFormat = renderTarget->GetDescription().ImageFormat;
			newTexDesc.SampleCount = renderTarget->GetDescription().SampleCount;
			resizedDescs.push_back(newTexDesc);

			RenderDevice::DestroyTexture(renderTarget);
		}

		m_Description.RenderTargets = { nullptr };

		GLuint drawbuffer = 0;
		for (int i = 0; i < resizedDescs.size(); i++)
		{
			const Ref<Texture>& renderTarget = RenderDevice::CreateTexture(resizedDescs[i]);
			const AttachmentType& attachment = m_Description.Attachments[i];

			GLenum openglAttachment = AttachmentTypeToOpenGLAttachment(attachment);
			if (openglAttachment == GL_COLOR_ATTACHMENT0)
				openglAttachment += drawbuffer;

			glNamedFramebufferTexture(m_RendererID, openglAttachment, renderTarget->GetRendererID(), 0);
			m_RenderTargetCount++;

			if (attachment == AttachmentType::Color)
				drawbuffer++;
		}

		LUM_CORE_TRACE("Framebuffer Resized!");
	}
}