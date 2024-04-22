#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
#pragma region PipelineDepthState
	enum class DepthComparison : u8
	{
		None,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always
	};

	struct DepthBoundaries
	{
		float MinDepth = 0.0f;
		float MaxDepth = 1.0f;
	};

	struct PipelineDepthState
	{
		DepthBoundaries DepthRange{};
		DepthComparison DepthFunc = DepthComparison::Less;
		bool DepthTest = true;
		bool DepthWrite = true;
	};
#pragma endregion

#pragma region PipelineBlendState
	enum class BlendFactor : u8
	{
		Zero, One,
		SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor,
		SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha,
		ConstantColor, OneMinusConstantColor,
		ConstantAlpha, OneMinusConstantAlpha,
	};

	enum class BlendEquation : u8
	{
		Add, Subtract, ReverseSubtract, Minimum, Maximum,
		SrcMinusDst = Subtract,
		DstMinusSrc = ReverseSubtract,
	};

	struct BlendConstants
	{
		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
		float A = 0.0f;
	};

	struct PipelineBlendState
	{
		BlendConstants ConstantColor{};
		BlendFactor SrcColorFactor = BlendFactor::One;
		BlendFactor DstColorFactor = BlendFactor::Zero;
		BlendFactor SrcAlphaFactor = BlendFactor::One;
		BlendFactor DstAlphaFactor = BlendFactor::Zero;
		BlendEquation Equation = BlendEquation::Add;
		bool BlendEnable = true;
	};
#pragma endregion

#pragma region PipelinePolygonState
	enum class PolygonRasterMode : u8
	{
		Fill,
		Line,
		Point
	};

	enum class CullingMode : u8
	{
		Front,
		Back,
		FrontAndBack
	};

	enum class FrontFaceMode : u8
	{
		CounterClockwise,
		Clockwise
	};

	struct PipelinePolygonState
	{
		PolygonRasterMode PolygonMode = PolygonRasterMode::Fill;
		CullingMode CullMode = CullingMode::Back;
		FrontFaceMode FrontFace = FrontFaceMode::CounterClockwise;
		bool CullEnable = true;
	};
#pragma endregion
}