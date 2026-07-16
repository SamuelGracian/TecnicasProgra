#pragma once 
#include <d3d11.h>
#include"Graphics/RasterizerState.h"

class Dx11RasterizerState : public RasterizerState
{
public:

	Dx11RasterizerState() :
		m_rasterizer(nullptr), m_rasterizerDesc(nullptr)
	{ }

	~Dx11RasterizerState() = default;


	ID3D11RasterizerState* m_rasterizer;
	const D3D11_RASTERIZER_DESC* m_rasterizerDesc;
};