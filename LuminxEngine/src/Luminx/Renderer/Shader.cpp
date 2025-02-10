#include "lumpch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Luminx
{
	static GLenum ShaderTypeToGLenum(ShaderType type)
	{
		switch (type)
		{
			case ShaderType::None:			return 0;
			case ShaderType::Vertex:		return GL_VERTEX_SHADER;
			case ShaderType::Geometry:		return GL_GEOMETRY_SHADER;
			case ShaderType::Fragment:		return GL_FRAGMENT_SHADER;

			default:
				return 0;
		}
	}

	static std::string ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				LUM_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			LUM_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	static ShaderStreamMap PreProcess(const std::string& source)
	{
		ShaderStreamMap shaderSources;
		const char* typeToken = "#shader";

		std::stringstream stream(source);
		std::string line;

		ShaderType type = ShaderType::None;
		while (getline(stream, line))
		{
			if (line.find(typeToken) != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::Vertex;
				else if (line.find("geometry") != std::string::npos)
					type = ShaderType::Geometry;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::Fragment;
			}
			else
			{
				shaderSources[type] << line << '\n';
			}
		}

		return shaderSources;
	}

	static GLuint Compile(const std::unordered_map<ShaderType, std::stringstream>& shaderSources)
	{
		GLuint program = glCreateProgram();
		std::array<GLuint, (int)ShaderType::Count - 1> glShaderIDs{};  // ShaderType::None is also a shader type
		int shaderIndex = 0;
		for (auto& [type, stream] : shaderSources) {
			if (type == ShaderType::None) continue;

			GLenum shaderType = ShaderTypeToGLenum(type);

			GLuint shaderID = glCreateShader(shaderType);
			std::string source = stream.str();
			const char* c_source = source.c_str();
			glShaderSource(shaderID, 1, &c_source, 0);
			glCompileShader(shaderID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shaderID);

				LUM_CORE_ERROR("{0}", infoLog.data());
				LUM_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shaderID);
			glShaderIDs[shaderIndex++] = shaderID;
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			for (const GLuint& id : glShaderIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}

			glDeleteProgram(program);
			program = 0;

			LUM_CORE_ERROR("{0}", infoLog.data());
			LUM_CORE_ASSERT(false, "Shader linking failure!");
		}

		for (const GLuint& id : glShaderIDs)
		{
			if (id == 0) continue;

			glDetachShader(program, id);
			glDeleteShader(id);
		}

		return program;
	}

	Shader::Shader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		ShaderStreamMap shaderSources = PreProcess(source);
		m_RendererID = Compile(shaderSources);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Use()
	{
		glUseProgram(m_RendererID);
	}

	GLint Shader::GetUniformLocation(const std::string& name) const
	{
		if (auto locationCache = m_UniformLocationCache.find(name);
			locationCache != m_UniformLocationCache.end()) return locationCache->second;

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		m_UniformLocationCache[name] = location;
		return location;
	}

	void Shader::SetBool(const std::string& name, const bool& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1i(m_RendererID, location, (int)value);
	}

	void Shader::SetInt(const std::string& name, const int& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1i(m_RendererID, location, value);
	}

	void Shader::SetIntArray(const std::string& name, const int*& values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1iv(m_RendererID, location, count, values);
	}

	void Shader::SetFloat(const std::string& name, const float& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform1f(m_RendererID, location, value);
	}

	void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform2f(m_RendererID, location, value.x, value.y);
	}

	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform3f(m_RendererID, location, value.x, value.y, value.z);
	}

	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform4f(m_RendererID, location, value.x, value.y, value.z, value.w);
	}

	void Shader::SetColor(const std::string& name, const glm::vec4& color)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniform4f(m_RendererID, location, color.r, color.g, color.b, color.a);
	}

	void Shader::SetMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniformMatrix3fv(m_RendererID, location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = GetUniformLocation(name);
		glProgramUniformMatrix4fv(m_RendererID, location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}