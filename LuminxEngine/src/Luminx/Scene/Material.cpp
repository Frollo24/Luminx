#include "lumpch.h"
#include "Material.h"

#include "Luminx/Renderer/RenderDevice.h"

namespace Luminx
{
	Material::Material(const Ref<Shader>& shader, const glm::vec4& mainColor)
		: m_Shader(shader), m_MainTexture(Texture::CreateWhiteTexture()), m_MainColor(mainColor)
	{
	}

	Material::Material(const Ref<Shader>& shader, const Ref<Texture>& mainTexture)
		: m_Shader(shader), m_MainTexture(mainTexture)
	{
		if (!m_MainTexture)
			m_MainTexture = Texture::CreateWhiteTexture();
	}

	Material::~Material()
	{
		m_MainTexture = nullptr;
	}
}