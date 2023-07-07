#pragma once

#include "Luminx/Core/Base.h"
#include "Luminx/Renderer/RenderEntity.h"

#include <glm/glm.hpp>

namespace Luminx
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		bool operator==(const Vertex& other) const {
			return Position == other.Position
				&& Normal == other.Normal
				&& TexCoord == other.TexCoord
				&& Tangent == other.Tangent
				&& Bitangent == other.Bitangent;
		}
	};

	class Model
	{
	public:
		Model(const std::string& filepath);
		~Model();

		void Render();

	private:
		void BuildTinyObjModel();

	private:
		Ref<RenderEntity> m_RenderEntity = nullptr;
		std::string m_Filepath;
		u32 m_VertexCount = 0;
		u32 m_IndexCount = 0;
	};
}

