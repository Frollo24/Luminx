#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
	// Not pretty, but it works at the moment
	namespace Utils
	{
		void* LoadImageFromDisk(const std::string_view& path, i32& width, i32& height, i32& channels);
		void FreeImageData(void* data);
	}

	enum class ImageType
	{
		Image1D,
		Image2D,
		Image3D,
	};

	struct ImageExtent
	{
		u32 width;
		u32 height;
		u32 depth;
	};

	enum class ImageFormat
	{
		None = 0,
		RGB8,
		RGBA8
	};

	enum class TextureFilterMode
	{
		Nearest = 0,
		Linear,
		NearestMipmap,
		LinearMipmap
	};

	enum class TextureWrapMode
	{
		Repeat = 0,
		ClampEdge,
		ClampBorder,
		Mirror,
		MirrorOnce
	};

	struct TextureDescription
	{
		ImageType ImageType = ImageType::Image2D;
		ImageExtent ImageExtent{1, 1, 1};
		ImageFormat ImageFormat = ImageFormat::RGBA8;
		TextureFilterMode FilterMode = TextureFilterMode::Linear;
		TextureWrapMode WrapMode = TextureWrapMode::Repeat;
		bool GenerateMipmaps = true;
	};

	class Texture
	{
	public:
		Texture(const TextureDescription& desc);
		~Texture();

		inline u32 GetRendererID() const { return m_RendererID; }
		inline const TextureDescription& GetDescription() const { return m_Description; }

		void SetData(const void* data);
		void BindTextureUnit(u32 textureUnit);

	private:
		TextureDescription m_Description{};
		u32 m_RendererID = 0;
	};
}

