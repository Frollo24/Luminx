#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
	// Not pretty, but it works at the moment
	namespace Utils
	{
		void* LoadImageFromDisk(const std::string_view& path, i32& width, i32& height, i32& channels);
		void* LoadHDRImageFromDisk(const std::string_view& path, i32& width, i32& height, i32& channels);
		void FreeImageData(void* data);
	}

	enum class ImageType : u8
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

	enum class ImageFormat : u8
	{
		None = 0,
		RGB8,
		RGBA8,
		RGB9E5,
		RGB16F
	};

	enum class TextureFilterMode : u8
	{
		Nearest = 0,
		Linear,
		NearestMipmap,
		LinearMipmap
	};

	enum class TextureWrapMode : u8
	{
		Repeat = 0,
		ClampEdge,
		ClampBorder,
		Mirror,
		MirrorOnce
	};

	enum class SampleCount : u8
	{
		Count64 = 64,
		Count32 = 32,
		Count16 = 16,
		Count8 = 8,
		Count4 = 4,
		Count2 = 2,
		Count1 = 1,
	};

	struct TextureDescription
	{
		ImageExtent ImageExtent{1, 1, 1};
		ImageType ImageType = ImageType::Image2D;
		ImageFormat ImageFormat = ImageFormat::RGBA8;
		TextureFilterMode FilterMode = TextureFilterMode::Linear;
		TextureWrapMode WrapMode = TextureWrapMode::Repeat;
		SampleCount SampleCount = SampleCount::Count1;
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

		static Ref<Texture> CreateWhiteTexture();

	private:
		TextureDescription m_Description{};
		u32 m_RendererID = 0;
	};
}

