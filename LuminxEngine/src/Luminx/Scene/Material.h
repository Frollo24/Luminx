#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/Texture.h"
#include "Luminx/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Luminx
{
	class Material
	{
	public:
		inline const glm::vec4& GetMainColor() const { return m_MainColor; }
		inline void SetMainColor(const glm::vec4& mainColor) { m_MainColor = mainColor; }

		inline const Ref<Texture>& GetMainTexture() const { return m_MainTexture; }
		inline void SetMainTexture(const Ref<Texture>& mainTexture) { m_MainTexture = mainTexture; }

		inline const Ref<Shader>& GetShader() const { return m_Shader; }
		inline void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }

		virtual void Select() = 0;

	protected:
		Material(const Ref<Shader>& shader, const glm::vec4& mainColor);
		Material(const Ref<Shader>& shader, const Ref<Texture>& mainTexture);
		virtual ~Material();

	private:
		Ref<Shader> m_Shader = nullptr;
		Ref<Texture> m_MainTexture = nullptr;
		glm::vec4 m_MainColor = glm::vec4(1.0f);
	};
}

