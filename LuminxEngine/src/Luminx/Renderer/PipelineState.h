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
	enum class ColorWriteMask : u8
	{
		ColorWriteMaskR = LUM_BIT(0), // 1 (0x0001)
		ColorWriteMaskG = LUM_BIT(1), // 2 (0x0010)
		ColorWriteMaskB = LUM_BIT(2), // 4 (0x0100)
		ColorWriteMaskA = LUM_BIT(3), // 8 (0x1000)
		ColorWriteMaskAll = ColorWriteMaskR | ColorWriteMaskG | ColorWriteMaskB | ColorWriteMaskA, // 15 (0x1111)
	};

	enum class BlendFactor : u8
	{
		Zero, One,
		SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor,
		SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha,
		ConstantColor, OneMinusConstantColor,
		ConstantAlpha, OneMinusConstantAlpha,
	};

	enum class BlendOperation : u8
	{
		Add, Subtract, ReverseSubtract, Minimum, Maximum,
		SrcMinusDst = Subtract,
		DstMinusSrc = ReverseSubtract,
	};

	struct BlendingEquation
	{
		BlendFactor SrcFactor = BlendFactor::One;
		BlendFactor DstFactor = BlendFactor::Zero;
		BlendOperation Operation = BlendOperation::Add;
	};

	struct BlendAttachment
	{
		BlendingEquation ColorEquation{};
		BlendingEquation AlphaEquation{};
		ColorWriteMask ColorWriteMask = ColorWriteMask::ColorWriteMaskAll;
		bool BlendEnable = false;
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
		std::array<BlendAttachment, 8> BlendAttachments{};
		BlendConstants ConstantColor{};
	};

	LUM_DEFINE_ENUM_FLAG_OPERATORS(ColorWriteMask);
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