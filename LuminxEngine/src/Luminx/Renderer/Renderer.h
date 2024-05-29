#pragma once

#include "Luminx/Renderer/VertexArray.h"
#include "Luminx/Renderer/RenderEntity.h"
#include "Luminx/Renderer/Pipeline.h"
#include "Luminx/Scene/Camera.h"

namespace Luminx
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadSkyboxPipeline(const Ref<Shader>& skyboxShader);

		static void BeginFrame();
		static void EndFrame();

		static void Submit(const Ref<RenderEntity>& renderEntity);
		static void DrawSkybox(const Ref<Camera>& camera);
	};
}
