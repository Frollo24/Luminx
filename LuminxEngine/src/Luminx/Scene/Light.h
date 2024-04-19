#pragma once

#include "Luminx/Core/Base.h"
#include <glm/glm.hpp>

namespace Luminx
{
	enum class LightType
	{
		Directional
	};

	class Light
	{
	public:
		Light(LightType type, glm::vec3 color, float intensity);
		~Light() = default;

		inline glm::vec4 GetLightingData() const { return glm::vec4(m_Color, m_Intensity); }

		inline void SetPosition(const glm::vec3& position) { m_Position = position; }
		inline const glm::vec3& GetPosition() { return m_Position; }

		inline void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }
		inline const glm::vec3& GetDirection() { return m_Direction; }

	private:
		LightType m_Type = LightType::Directional;
		glm::vec3 m_Color;
		float m_Intensity;
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	};
}

