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

    Texture::Texture(const TextureDescription& desc)
        : m_Description(desc)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

        // TODO: Handle filtering and wrapping
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // TODO: Handle multiple texture extents
        GLenum internalFormat = ImageFormatToOpenGLInternalFormat(desc.ImageFormat);
        glTextureStorage2D(m_RendererID, 1, internalFormat, desc.ImageExtent.width, desc.ImageExtent.height);
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
