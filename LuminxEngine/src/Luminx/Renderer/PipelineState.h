#pragma once

#include "Luminx/Core/Base.h"

namespace Luminx
{
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
}