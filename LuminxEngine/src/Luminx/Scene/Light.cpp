#include "lumpch.h"
#include "Light.h"

namespace Luminx
{
	Light::Light(LightType type, glm::vec3 color, float intensity) :
		m_Type(type), m_Color(color), m_Intensity(intensity)
	{
		if (m_Intensity < 0.0f)
			m_Intensity = 0.0f;

		glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
	}
}