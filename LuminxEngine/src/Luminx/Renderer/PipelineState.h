#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
#pragma region PipelineDepthState
	enum class DepthComparison
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
		bool DepthTest = true;
		bool DepthWrite = true;
		DepthComparison DepthFunc = DepthComparison::Less;
		DepthBoundaries DepthRange{};
	};
#pragma endregion

#pragma region PipelineBlendState
	enum class BlendFactor
	{
		Zero, One,
		SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor,
		SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha,
		ConstantColor, OneMinusConstantColor,
		ConstantAlpha, OneMinusConstantAlpha,
	};

	enum class BlendEquation
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
		bool BlendEnable = true;
		BlendFactor SrcColorFactor = BlendFactor::Zero;
		BlendFactor DstColorFactor = BlendFactor::One;
		BlendFactor SrcAlphaFactor = BlendFactor::Zero;
		BlendFactor DstAlphaFactor = BlendFactor::One;
		BlendEquation Equation = BlendEquation::Add;
		BlendConstants ConstantColor{};
	};
#pragma endregion
}