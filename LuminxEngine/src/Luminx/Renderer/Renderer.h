#pragma once

#include "Luminx/Renderer/VertexArray.h"
#include "Luminx/Renderer/RenderEntity.h"
#include "Luminx/Renderer/Shader.h"

namespace Luminx
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();
		static void Submit(const Ref<RenderEntity>& renderEntity);
	};
}
