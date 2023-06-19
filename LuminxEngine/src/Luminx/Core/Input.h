#pragma once

#include "Luminx/Core/Base.h"

#include "Luminx/Core/KeyCode.h"
#include "Luminx/Core/MouseCode.h"

namespace Luminx
{
	class Input
	{
	public:
		static bool IsKeyDown(KeyCode keycode);
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsKeyReleased(KeyCode keycode);
		static bool IsKeyUp(KeyCode keycode);

		static bool IsMouseButtonDown(MouseCode button);
		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode button);
		static bool IsMouseButtonUp(MouseCode button);

		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

		static void OnUpdate();

	private:
		static std::unordered_map<int, bool> s_KeyMaps;
		static std::unordered_map<int, bool> s_MouseMaps;
	};

}