#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/Common.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace Luminx
{
	enum class ClearFlags
	{
		None = 0,
		Color = LUM_BIT(0),
		Depth = LUM_BIT(1),
		Stencil = LUM_BIT(2),
		DepthStencil = Depth | Stencil,
		All = Color | DepthStencil,
	};

	LUM_DEFINE_ENUM_FLAG_OPERATORS(ClearFlags);

	struct ClearValues
	{
		glm::vec4 Color = glm::vec4(1.0f);
		float Depth = 1.0f;
		i32 Stencil = 0;
		ClearFlags ClearFlags = ClearFlags::None;
	};

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
		std::array<AttachmentType, MAX_TOTAL_ATTACHMENTS> Attachments = { AttachmentType::None };
		std::array<Ref<Texture>, MAX_TOTAL_ATTACHMENTS> RenderTargets = { nullptr };
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

		void Resize(u32 width, u32 height);

	private:
		FramebufferDescription m_Description{};
		u32 m_RenderTargetCount = 0;
		u32 m_RendererID = 0;
	};
}

