#pragma once

#include "Luminix/Core/Base.h"

#include "Luminix/Core/KeyCode.h"
#include "Luminix/Core/MouseCode.h"

namespace Luminix
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