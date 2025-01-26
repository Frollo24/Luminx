#pragma once

#include "Luminx/Core/Base.h"
#include <glm/glm.hpp>

namespace Luminx
{
	enum class LightType
	{
		Directional, Point, Spot
	};

	class Light
	{
	public:
		Light(LightType type, glm::vec3 color, float intensity);
		~Light() = default;

		inline glm::vec4 GetLightingData() const { return glm::vec4(m_Color, m_Intensity); }

		inline void SetColor(const glm::vec3& color) { m_Color = color; }
		inline const glm::vec3& GetColor() { return m_Color; }

		inline void SetIntensity(const float& intensity) { m_Intensity = intensity; }
		inline const float& GetIntensity() { return m_Intensity; }

		inline void SetPosition(const glm::vec3& position) { m_Position = position; }
		inline const glm::vec3& GetPosition() { return m_Position; }

		inline void SetRadius(const float& radius) { m_Radius = radius; }
		inline const float& GetRadius() { return m_Radius; }

		inline void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }
		inline const glm::vec3& GetDirection() { return m_Direction; }

		inline const float& GetPhi() { return m_Phi; }
		inline const float& GetGamma() { return m_Gamma; }

	private:
		LightType m_Type = LightType::Directional;
		glm::vec3 m_Color;
		float m_Intensity;
		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Radius = 10.0f;
		glm::vec3 m_Direction = glm::vec3(0.0f, -1.0f, 0.0f);
		float m_Phi = 25.0f; // Inner cone
		float m_Gamma = 35.0f; // Outer cone
	};
}

