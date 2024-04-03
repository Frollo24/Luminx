#pragma once

// For use by Luminx applications

// Core elements
#include "Luminx/Core/Application.h"
#include "Luminx/Core/Input.h"
#include "Luminx/Core/Layer.h"
#include "Luminx/Core/Log.h"
#include "Luminx/Core/Time.h"

// Events
#include "Luminx/Events/Event.h"
#include "Luminx/Events/WindowEvent.h"

// Renderer
#include "Luminx/Renderer/Renderer.h"
#include "Luminx/Renderer/RenderDevice.h"
#include "Luminx/Renderer/RenderCommand.h"

// Scene
#include "Luminx/Scene/Model.h"

// Window system
#include "Luminx/Window/WindowSystem.h"

#ifdef LUMINIX_ENTRY_POINT
// Entry Point
#include "Luminx/Core/EntryPoint.h"
#endif // LUMINIX_ENTRY_POINT