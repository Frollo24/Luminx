#include "lumpch.h"
#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Luminx
{
    static GLenum ImageFormatToOpenGLInternalFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::None:  return GL_NONE;
            case ImageFormat::RGB8:  return GL_RGB8;
            case ImageFormat::RGBA8: return GL_RGBA8;
            default:
                return GL_NONE;
        }
    }

    static GLenum ImageFormatToOpenGLFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::None:  return GL_NONE;
            case ImageFormat::RGB8:  return GL_RGB;
            case ImageFormat::RGBA8: return GL_RGBA;
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

    Texture::Texture(const TextureDescription& desc)
        : m_Description(desc)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

        GLenum filterMode = TextureFilterModeToOpenGLFilterMode(desc.FilterMode);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filterMode);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filterMode == GL_NEAREST ? filterMode : GL_LINEAR);

        GLenum wrapMode = TextureWrapModeToOpenGLWrapMode(desc.WrapMode);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, wrapMode);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, wrapMode);

        // TODO: Handle multiple texture extents
        GLenum internalFormat = ImageFormatToOpenGLInternalFormat(desc.ImageFormat);
        glTextureStorage2D(m_RendererID, 1, internalFormat, desc.ImageExtent.width, desc.ImageExtent.height);

        // TODO: Handle texture mipmapping
        if (desc.GenerateMipmaps)
            glGenerateTextureMipmap(m_RendererID);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void Texture::SetData(const void* data)
    {
        // TODO: handle multiple formats
        GLenum format = ImageFormatToOpenGLFormat(m_Description.ImageFormat);
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Description.ImageExtent.width, m_Description.ImageExtent.height, format, GL_UNSIGNED_BYTE, data);
    }

    void Texture::BindTextureUnit(u32 textureUnit)
    {
        glBindTextureUnit(textureUnit, m_RendererID);
    }
}

void* Luminx::Utils::LoadImageFromDisk(const std::string_view& path, i32& width, i32& height, i32& channels)
{
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    data = stbi_load(path.data(), &width, &height, &channels, 0);
    return data;
}

void Luminx::Utils::FreeImageData(void* data)
{
    stbi_image_free(data);
}
