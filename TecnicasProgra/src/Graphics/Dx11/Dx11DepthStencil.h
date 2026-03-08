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

	static UINT GetClearFlagDx11( const DepthStencilView::ClearFlags flag ) 
	{
		UINT returnFlag = 0;

		if ( HasFlag (flag, DepthStencilView::ClearFlags::Depth) )
		{
			returnFlag |= D3D11_CLEAR_FLAG:: D3D11_CLEAR_DEPTH;
		}

		if (HasFlag( flag , DepthStencilView::ClearFlags::Stencil ))
		{
			returnFlag |= D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL;
		}
		return returnFlag;
	}

	static bool HasFlag(DepthStencilView::ClearFlags flag, DepthStencilView::ClearFlags flagToCheck) 
	{
		return (static_cast<uint8_t> (flag) & static_cast<uint8_t> (flagToCheck)) != 0;
	}

	ID3D11DepthStencilView* m_depthStencilView;
};