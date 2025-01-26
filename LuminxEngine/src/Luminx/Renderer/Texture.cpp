#include "lumpch.h"
#include "Texture.h"
#include "RenderDevice.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

namespace Luminx
{
	static GLenum GetOpenGLTarget(const TextureDescription& desc)
	{
		if ((int)(desc.SampleCount) > 1)
		{
			switch (desc.ImageType)
			{
			case ImageType::Image2D: return GL_TEXTURE_2D_MULTISAMPLE;
			case ImageType::Image3D: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
			default:
				return GL_NONE;
			}
		}
		else
		{
			switch (desc.ImageType)
			{
			case ImageType::Image1D: return GL_TEXTURE_1D;
			case ImageType::Image2D: return GL_TEXTURE_2D;
			case ImageType::Image3D: return GL_TEXTURE_3D;
			case ImageType::Cubemap: return GL_TEXTURE_CUBE_MAP;
			default:
				return GL_NONE;
			}
		}
	}

	static GLenum ImageFormatToOpenGLInternalFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::None:    return GL_NONE;
		case ImageFormat::RGB8:    return GL_RGB8;
		case ImageFormat::RGBA8:   return GL_RGBA8;
		case ImageFormat::RGB9E5:  return GL_RGB9_E5;
		case ImageFormat::RG16F:   return GL_RG16F;
		case ImageFormat::RGB16F:  return GL_RGB16F;
		case ImageFormat::RGBA16F: return GL_RGBA16F;
		case ImageFormat::DEPTH32: return GL_DEPTH_COMPONENT32;
		default:
			return GL_NONE;
		}
	}

	static GLenum ImageFormatToOpenGLFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::None:    return GL_NONE;
		case ImageFormat::RGB8:    return GL_RGB;
		case ImageFormat::RGBA8:   return GL_RGBA;
		case ImageFormat::RGB9E5:  return GL_RGB;
		case ImageFormat::RG16F:   return GL_RG;
		case ImageFormat::RGB16F:  return GL_RGB;
		case ImageFormat::RGBA16F: return GL_RGBA;
		case ImageFormat::DEPTH32: return GL_DEPTH_COMPONENT;
		default:
			return GL_NONE;
		}
	}

	static GLenum ImageFormatToOpenGLType(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::None:    return GL_NONE;
		case ImageFormat::RGB8:    return GL_UNSIGNED_BYTE;
		case ImageFormat::RGBA8:   return GL_UNSIGNED_BYTE;
		case ImageFormat::RGB9E5:  return GL_UNSIGNED_BYTE;
		case ImageFormat::RG16F:   return GL_FLOAT;
		case ImageFormat::RGB16F:  return GL_FLOAT;
		case ImageFormat::RGBA16F: return GL_FLOAT;
		case ImageFormat::DEPTH32: return GL_FLOAT;
		default:
			return GL_NONE;
		}
	}

	static GLenum TextureFilterModeToOpenGLFilterMode(TextureFilterMode filterMode)
	{
		switch (filterMode)
		{
		case TextureFilterMode::Nearest:       return GL_NEAREST;
		case TextureFilterMode::Linear:        return GL_LINEAR;
		case TextureFilterMode::NearestMipmap: return GL_LINEAR_MIPMAP_NEAREST;
		case TextureFilterMode::LinearMipmap:  return GL_LINEAR_MIPMAP_LINEAR;
		default:
			return GL_NONE;
		}
	}

	static GLenum TextureWrapModeToOpenGLWrapMode(TextureWrapMode wrapMode)
	{
		switch (wrapMode)
		{
		case Luminx::TextureWrapMode::Repeat:      return GL_REPEAT;
		case Luminx::TextureWrapMode::ClampEdge:   return GL_CLAMP_TO_EDGE;
		case Luminx::TextureWrapMode::ClampBorder: return GL_CLAMP_TO_BORDER;
		case Luminx::TextureWrapMode::Mirror:      return GL_MIRRORED_REPEAT;
		case Luminx::TextureWrapMode::MirrorOnce:  return GL_MIRROR_CLAMP_TO_EDGE;
		default:
			return GL_NONE;
		}
	}

	static GLsizei CalculateMipmapLevels(const ImageExtent& extent)
	{
		double maxElement = std::max(std::max(extent.width, extent.height), extent.depth);
		GLsizei mipLevels = static_cast<GLsizei>(std::floor(std::log2(maxElement))) + 1;
		return mipLevels;
	}

	Texture::Texture(const TextureDescription& desc)
		: m_Description(desc)
	{
		GLenum target = GetOpenGLTarget(desc);
		glCreateTextures(target, 1, &m_RendererID);

		bool isMultisampled = (int)(desc.SampleCount) > 1;
		if (!isMultisampled)
		{
			GLenum filterMode = TextureFilterModeToOpenGLFilterMode(desc.FilterMode);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filterMode);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filterMode == GL_NEAREST ? filterMode : GL_LINEAR);

			GLenum wrapMode = TextureWrapModeToOpenGLWrapMode(desc.WrapMode);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, wrapMode);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, wrapMode);
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(desc.BorderColor));
		}

		GLsizei mipLevels = 1;
		if (desc.GenerateMipmaps)
			mipLevels = CalculateMipmapLevels(desc.ImageExtent);

		GLenum internalFormat = ImageFormatToOpenGLInternalFormat(desc.ImageFormat);
		const ImageExtent& extent = desc.ImageExtent;
		if (isMultisampled)
		{
			switch (desc.ImageType)
			{
			case ImageType::Image2D:
				glTextureStorage2DMultisample(m_RendererID, (GLsizei)desc.SampleCount, internalFormat, extent.width, extent.height, GL_FALSE);
				break;
			case ImageType::Image3D:
				glTextureStorage3DMultisample(m_RendererID, (GLsizei)desc.SampleCount, internalFormat, extent.width, extent.height, extent.depth, GL_FALSE);
				break;
			default:
				break;
			}
		}
		else
		{
			switch (desc.ImageType)
			{
			case ImageType::Image1D:
				glTextureStorage1D(m_RendererID, mipLevels, internalFormat, extent.width);
				break;
			case ImageType::Image2D:
			case ImageType::Cubemap:
				glTextureStorage2D(m_RendererID, mipLevels, internalFormat, extent.width, extent.height);
				break;
			case ImageType::Image3D:
				glTextureStorage3D(m_RendererID, mipLevels, internalFormat, extent.width, extent.height, extent.depth);
				break;
			default:
				break;
			}
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::SetData(const void* data)
	{
		GLenum format = ImageFormatToOpenGLFormat(m_Description.ImageFormat);
		GLenum type = ImageFormatToOpenGLType(m_Description.ImageFormat);

		switch (m_Description.ImageType)
		{
		case ImageType::Image1D:
			glTextureSubImage1D(m_RendererID, 0, 0, m_Description.ImageExtent.width, format, type, data);
			break;
		case ImageType::Image2D:
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Description.ImageExtent.width, m_Description.ImageExtent.height, format, type, data);
			break;
		case ImageType::Image3D:
			glTextureSubImage3D(m_RendererID, 0, 0, 0, 0, m_Description.ImageExtent.width, m_Description.ImageExtent.height, m_Description.ImageExtent.depth,
				format, type, data);
			break;
		case ImageType::Cubemap:
			for (int face = 0; face < 6; face++)
			{
				void* faceData = ((size_t**)(data))[face];
				glTextureSubImage3D(m_RendererID, 0, 0, 0, face, m_Description.ImageExtent.width, m_Description.ImageExtent.height, 1, format, type, faceData);
			}
		default:
			break;
		}

		if (m_Description.GenerateMipmaps)
			glGenerateTextureMipmap(m_RendererID);
	}

	void Texture::BindTextureUnit(u32 textureUnit)
	{
		glBindTextureUnit(textureUnit, m_RendererID);
	}

	Ref<Texture> Texture::CreateWhiteTexture()
	{
		TextureDescription whiteTextureDesc{};
		whiteTextureDesc.GenerateMipmaps = false;

		Ref<Texture> whiteTexture = RenderDevice::CreateTexture(whiteTextureDesc);
		u32 whiteData = 0xffffffff;
		whiteTexture->SetData(&whiteData);
		return whiteTexture;
	}
}

void* Luminx::Utils::LoadImageFromDisk(const std::string_view& path, i32& width, i32& height, i32& channels, bool flipOnLoad)
{
	stbi_set_flip_vertically_on_load(flipOnLoad);
	stbi_uc* data = nullptr;
	data = stbi_load(path.data(), &width, &height, &channels, 0);
	return data;
}

void* Luminx::Utils::LoadHDRImageFromDisk(const std::string_view& path, i32& width, i32& height, i32& channels, bool flipOnLoad)
{
	stbi_set_flip_vertically_on_load(flipOnLoad);
	float* data = nullptr;
	data = stbi_loadf(path.data(), &width, &height, &channels, 0);
	return data;
}

std::array<void*, 6> Luminx::Utils::LoadCubemapFromDisk(const std::string_view& folder, const std::vector<std::string_view>& facesNames, i32& width, i32& height, i32& channels)
{
	std::array<void*, 6> cubemapData{};
	for (size_t i = 0; i < facesNames.size(); i++)
	{
		std::string path = std::string(folder) + std::string(facesNames[i]);
		void* faceData = LoadImageFromDisk(path, width, height, channels, false);
		cubemapData[i] = faceData;
	}
	return cubemapData;
}

void Luminx::Utils::FreeImageData(void* data)
{
	stbi_image_free(data);
}

void Luminx::Utils::FreeCubemapData(const std::array<void*, 6>& cubemapData)
{
	for (void* faceData : cubemapData)
	{
		FreeImageData(faceData);
		faceData = nullptr;
	}
}
