#include "lumpch.h"
#include "PhongMaterial.h"

namespace Luminx
{
	PhongMaterial::PhongMaterial(const Ref<Shader>& shader, const std::array<glm::vec4, 3>& colors)
		: Material(shader, colors[0]), m_Colors(colors)
	{
		for (auto& texture : m_Textures)
			texture = Texture::CreateWhiteTexture();
		m_NormalTexture = Texture::CreateWhiteTexture();
	}

	PhongMaterial::PhongMaterial(const Ref<Shader>& shader, const std::array<Ref<Texture>, 3>& textures)
		: Material(shader, textures[0]), m_Textures(textures)
	{
		for (auto& texture : m_Textures)
			if (!texture)
				texture = Texture::CreateWhiteTexture();
	}

	void PhongMaterial::Select()
	{
		for (int i = 0; i < m_Textures.size(); i++)
			if (m_Textures[i])
				m_Textures[i]->BindTextureUnit(i);

		if (m_NormalTexture)
			m_NormalTexture->BindTextureUnit(3);

		const auto& shader = GetShader();
		if (shader)
		{
			// Setting colors
			shader->SetFloat4("u_Material.diffuse", m_Colors[0]);
			shader->SetFloat4("u_Material.specular", m_Colors[1]);
			shader->SetFloat4("u_Material.emissive", m_Colors[2]);

			// Setting glossiness
			shader->SetFloat("u_Material.glossiness", m_Glossiness);
		}
	}
}