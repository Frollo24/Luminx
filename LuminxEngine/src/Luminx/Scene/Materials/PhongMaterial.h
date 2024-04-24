#pragma once
#include "Luminx/Scene/Material.h"

namespace Luminx
{
	class PhongMaterial : public Material
	{
	public:
		PhongMaterial(const Ref<Shader>& shader, const std::array<glm::vec4, 3>& colors);
		PhongMaterial(const Ref<Shader>& shader, const std::array<Ref<Texture>, 3>& textures);

		virtual void Select() override;

	private:
		std::array<glm::vec4, 3> m_Colors = { glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f) };
		std::array<Ref<Texture>, 3> m_Textures{ nullptr, nullptr, nullptr };
	};
}
