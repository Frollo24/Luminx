#pragma once

#include "Luminx/Core/Base.h"
#include "Texture.h"

namespace Luminx
{
	enum class AttachmentType
	{
		None = 0,
		Color = 1,
		DepthStencil = 2,
		Depth = 3,
		Stencil = 4,
	};

	struct FramebufferDescription
	{
		std::vector<AttachmentType> Attachments;
		std::vector<Ref<Texture>> RenderTargets;
		u32 Width = 0;
		u32 Height = 0;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferDescription& desc);
		~Framebuffer();

		inline u32 GetRendererID() const { return m_RendererID; }
		inline const FramebufferDescription& GetDescription() const { return m_Description; }

		void Bind();
		void Unbind();

		// TODO: handle framebuffer resize

	private:
		FramebufferDescription m_Description{};
		u32 m_RendererID = 0;
	};
}

