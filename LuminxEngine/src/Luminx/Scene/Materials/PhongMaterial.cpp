#include "lumpch.h"
#include "PhongMaterial.h"

namespace Luminx
{
	PhongMaterial::PhongMaterial(const Ref<Shader>& shader, const std::array<glm::vec4, 3>& colors)
		: Material(shader, colors[0]), m_Colors(colors)
	{
	}

	PhongMaterial::PhongMaterial(const Ref<Shader>& shader, const std::array<Ref<Texture>, 3>& textures)
		: Material(shader, textures[0]), m_Textures(textures)
	{
	}

	void PhongMaterial::Select()
	{
		for (int i = 0; i < m_Textures.size(); i++)
			if (m_Textures[i])
				m_Textures[i]->BindTextureUnit(i);

		GetShader()->SetInt("u_DiffuseTexture", 0);
		GetShader()->SetInt("u_SpecularTexture", 1);
		GetShader()->SetInt("u_EmissiveTexture", 2);
	}
}