#pragma once
#include "Luminx/Scene/Material.h"

namespace Luminx
{
	class PhongMaterial : public Material
	{
	public:
		PhongMaterial(const Ref<Shader>& shader, const std::array<glm::vec4, 3>& colors);
		PhongMaterial(const Ref<Shader>& shader, const std::array<Ref<Texture>, 3>& textures);

		inline void SetDiffuseColor(const glm::vec4& diffuseColor) { m_Colors[0] = diffuseColor; }
		inline void SetSpecularColor(const glm::vec4& specularColor) { m_Colors[1] = specularColor; }
		inline void SetEmissiveColor(const glm::vec4& emissiveColor) { m_Colors[2] = emissiveColor; }

		inline void SetDiffuseTexture(const Ref<Texture>& diffuseTexture) { m_Textures[0] = diffuseTexture; }
		inline void SetSpecularTexture(const Ref<Texture>& specularTexture) { m_Textures[1] = specularTexture; }
		inline void SetEmissiveTexture(const Ref<Texture>& emissiveTexture) { m_Textures[2] = emissiveTexture; }
		inline void SetNormalTexture(const Ref<Texture>& normalTexture) { m_NormalTexture = normalTexture; }

		inline const glm::vec4& GetDiffuseColor() const { return m_Colors[0]; }
		inline const glm::vec4& GetSpecularColor() const { return m_Colors[1]; }
		inline const glm::vec4& GetEmissiveColor() const { return m_Colors[2]; }

		inline const Ref<Texture>& GetDiffuseTexture() const { return m_Textures[0]; }
		inline const Ref<Texture>& GetSpecularTexture() const { return m_Textures[1]; }
		inline const Ref<Texture>& GetEmissiveTexture() const { return m_Textures[2]; }
		inline const Ref<Texture>& GetNormalTexture() const { return m_NormalTexture; }

		inline void SetGlossiness(const float& glossiness) { m_Glossiness = glossiness; }
		inline const float& GetGlossiness() const { return m_Glossiness; }

		virtual void Select() override;

	private:
		std::array<glm::vec4, 3> m_Colors = { glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f) };
		std::array<Ref<Texture>, 3> m_Textures{ nullptr, nullptr, nullptr };
		Ref<Texture> m_NormalTexture = nullptr;
		float m_Glossiness = 0.5f;
	};
}
