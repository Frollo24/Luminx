#pragma once

#include "Luminx/Core/Base.h"
#include <glm/glm.hpp>

namespace Luminx
{
	namespace Utils
	{
		static glm::mat4 DefaultPerspective();
	}

	class Camera
	{
	public:
		Camera(const glm::mat4& projection) : m_Projection(projection) {}
		Camera(const glm::mat4& view, const glm::mat4& projection) : m_View(view), m_Projection(projection) {}
		~Camera() = default;

		static Ref<Camera> LookAt(const glm::vec3& origin, const glm::vec3& target, const glm::vec3& up);
		static Ref<Camera> HorizontalCoords(const f32& azimuth, const f32& elevation, const f32& distance);

		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline void SetProjection(const glm::mat4& proj) { m_Projection = proj; RecalculateViewProjMatrix(); }

		inline const glm::mat4& GetView() const { return m_View; }
		inline void SetView(const glm::mat4& view) { m_View = view; RecalculateViewProjMatrix(); }

		inline const glm::mat4& GetViewProjection() const { return m_ViewProj; }
		inline glm::vec3 GetPosition() const { return glm::vec3(glm::inverse(m_View) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); }

		void Update();

	protected:
		void RecalculateViewProjMatrix() { m_ViewProj = m_Projection * m_View; }

	private:
		glm::mat4 m_View = glm::mat4(1.0f);
		glm::mat4 m_Projection;
		glm::mat4 m_ViewProj = m_Projection * m_View;
	};
}

