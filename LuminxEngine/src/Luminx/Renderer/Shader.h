#pragma once

#include "Luminx/Core/Base.h"

#include <glm/glm.hpp>

typedef int GLint;

namespace Luminx
{
	enum class ShaderType
	{
		None = 0,
		Vertex,
		Fragment,
		Count
	};

	using ShaderStreamMap = std::unordered_map<ShaderType, std::stringstream>;

	class Shader
	{
	public:
		Shader(const std::string& filepath);
		~Shader();

		void Use();

		void SetBool(const std::string& name, const bool& value);
		void SetInt(const std::string& name, const int& value);
		void SetIntArray(const std::string& name, const int*& values, uint32_t count);
		void SetFloat(const std::string& name, const float& value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetColor(const std::string& name, const glm::vec4& color);
		void SetMat3(const std::string& name, const glm::mat3& matrix);
		void SetMat4(const std::string& name, const glm::mat4& matrix);

	private:
		mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
		u32 m_RendererID = 0;
		GLint GetUniformLocation(const std::string& name) const;
	};
}

