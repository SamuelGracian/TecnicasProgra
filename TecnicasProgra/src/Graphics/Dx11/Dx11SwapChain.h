#pragma once
#include <d3d11_1.h>
#include <stdint.h>
#include <memory>
#include "Graphics/SwapChain.h"

class RenderTargetView;

class Dx11SwapChain : public SwapChain
{
public:
	Dx11SwapChain() : m_swapChain(nullptr), m_BackBufferRT(nullptr) {}

	~Dx11SwapChain() = default;

	void Present(uint32_t SyncInterval, uint32_t Flags)  override;

	std::shared_ptr<RenderTargetView> GetRenderTargetView () override;
			 
	IDXGISwapChain* m_swapChain;

	ID3D11RenderTargetView* m_BackBufferRT;
};