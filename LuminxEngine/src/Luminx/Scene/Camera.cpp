#include "lumpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Luminx
{
	static const glm::mat4 default_projection = glm::perspective(glm::radians(39.3077f), 16.0f / 9.0f, 0.1f, 20.0f);

	namespace Utils
	{
		static glm::mat4 DefaultPerspective()
		{
			return default_projection;
		}
	}

	Ref<Camera> Camera::LookAt(const glm::vec3& origin, const glm::vec3& target, const glm::vec3& up)
	{
		glm::mat4& view = glm::lookAt(origin, target, up);
		glm::mat4 projection = Utils::DefaultPerspective();
		return CreateRef<Camera>(view, projection);
	}

	Ref<Camera> Camera::HorizontalCoords(const f32& azimuth, const f32& elevation, const f32& distance)
	{
		glm::mat4 azimuth_rot = glm::rotate(glm::mat4(1.0f), glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 elevation_rot = glm::rotate(glm::mat4(1.0f), glm::radians(elevation), glm::vec3(1.0, 0.0f, 0.0f));
		glm::mat4 distance_tra = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -distance));
		glm::mat4 view = distance_tra * elevation_rot * azimuth_rot;
		glm::mat4 projection = Utils::DefaultPerspective();
		return CreateRef<Camera>(view, projection);
	}

	void Camera::Update()
	{
		if (Input::IsKeyPressed(KeyCode::W))
			m_View = glm::translate(glm::mat4(1.0f), (float)Time::DeltaTime * glm::vec3(0.0f, 0.0f, 4.0f)) * m_View;
		if (Input::IsKeyPressed(KeyCode::S))
			m_View = glm::translate(glm::mat4(1.0f), (float)Time::DeltaTime * glm::vec3(0.0f, 0.0f, -4.0f)) * m_View;
		if (Input::IsKeyPressed(KeyCode::A))
			m_View = glm::translate(glm::mat4(1.0f), (float)Time::DeltaTime * glm::vec3(4.0f, 0.0f, 0.0f)) * m_View;
		if (Input::IsKeyPressed(KeyCode::D))
			m_View = glm::translate(glm::mat4(1.0f), (float)Time::DeltaTime * glm::vec3(-4.0f, 0.0f, 0.0f)) * m_View;
		if (Input::IsKeyPressed(KeyCode::Q))
			m_View = glm::translate(glm::mat4(1.0f), (float)Time::DeltaTime * glm::vec3(0.0f, 4.0f, 0.0f)) * m_View;
		if (Input::IsKeyPressed(KeyCode::E))
			m_View = glm::translate(glm::mat4(1.0f), (float)Time::DeltaTime * glm::vec3(0.0f, -4.0f, 0.0f)) * m_View;

		if (Input::IsKeyPressed(KeyCode::Left))
			m_View = glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f) * (float)Time::DeltaTime, glm::vec3(m_View[1])) * m_View;
		if (Input::IsKeyPressed(KeyCode::Right))
			m_View = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f) * (float)Time::DeltaTime, glm::vec3(m_View[1])) * m_View;

		if (Input::IsKeyPressed(KeyCode::Up))
			m_View = glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f) * (float)Time::DeltaTime, glm::vec3(1.0f, 0.0f, 0.0f)) * m_View;
		if (Input::IsKeyPressed(KeyCode::Down))
			m_View = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f) * (float)Time::DeltaTime, glm::vec3(1.0f, 0.0f, 0.0f)) * m_View;

		RecalculateViewProjMatrix();
	}
}