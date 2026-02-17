#pragma once
#include<d3d11.h>

#include"Graphics/RenderTargetView.h"

class Dx11RenderTargetView : public RenderTargetView
{
public:
	Dx11RenderTargetView()
		: m_renderTargetView(nullptr)
	{ }

	~Dx11RenderTargetView() = default;

	ID3D11RenderTargetView* m_renderTargetView;
};