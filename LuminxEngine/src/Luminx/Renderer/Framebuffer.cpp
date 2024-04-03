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
		for (int i = 0; i < m_Description.RenderTargets.size(); i++)
		{
			const Ref<Texture>& renderTarget = m_Description.RenderTargets[i];
			const AttachmentType& attachment = m_Description.Attachments[i];
			glNamedFramebufferTexture(m_RendererID, AttachmentTypeToOpenGLAttachment(attachment), renderTarget->GetRendererID(), 0);
		}
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
		resizedDescs.reserve(m_Description.RenderTargets.size());

		for (auto& renderTarget : m_Description.RenderTargets)
		{
			TextureDescription newTexDesc{};
			newTexDesc.ImageExtent = { width, height, 1 };
			newTexDesc.ImageFormat = renderTarget->GetDescription().ImageFormat;
			newTexDesc.SampleCount = renderTarget->GetDescription().SampleCount;
			resizedDescs.push_back(newTexDesc);

			RenderDevice::DestroyTexture(renderTarget);
		}

		m_Description.RenderTargets.clear();

		for (int i = 0; i < resizedDescs.size(); i++)
		{
			const Ref<Texture>& renderTarget = RenderDevice::CreateTexture(resizedDescs[i]);
			const AttachmentType& attachment = m_Description.Attachments[i];
			glNamedFramebufferTexture(m_RendererID, AttachmentTypeToOpenGLAttachment(attachment), renderTarget->GetRendererID(), 0);
			m_Description.RenderTargets.push_back(renderTarget);
		}

		LUM_CORE_TRACE("Framebuffer Resized!");
	}
}