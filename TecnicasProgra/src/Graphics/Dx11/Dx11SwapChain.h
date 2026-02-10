#pragma once
#include "Graphics/SwapChain.h"
#include <d3d11_1.h>

class Dx11SwapChain : public SwapChain
{
public:
	Dx11SwapChain() : m_swapChain(nullptr), m_BackBUfferRT(nullptr) {}

	~Dx11SwapChain() = default;
			 
	IDXGISwapChain* m_swapChain;

	ID3D11RenderTargetView* m_BackBUfferRT;
};