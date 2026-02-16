#pragma once

#include<d3d11.h>

#include "Graphics/DepthStencilView.h"
#include "Graphics/Dx11/DX11GraphicsAPI.h"

class Dx11DepthStencilView : public DepthStencilView
{

public:
	Dx11DepthStencilView() :
		m_depthStencilView(nullptr)
	{ }

	~Dx11DepthStencilView() = default;

	ID3D11DepthStencilView* m_depthStencilView;
};