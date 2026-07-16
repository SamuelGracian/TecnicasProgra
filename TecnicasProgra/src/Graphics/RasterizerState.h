#pragma once
#include "GraphicGenerals.h"

class RasterizerState
{ 
public:
	RasterizerState() = default;
	~RasterizerState() = default;

	FILL_MODE::K	FillMode;
	CULL_MODE::K	CullMode;
	bool            DepthClipEnable;
};