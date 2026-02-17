#include "Graphics/Dx11/Dx11SwapChain.h"

#include"Graphics/Dx11/Dx11RenderTargetView.h"

void Dx11SwapChain::Present(uint32_t SyncInterval, uint32_t Flags)
{
	m_swapChain->Present(SyncInterval, Flags);
}

std::shared_ptr<RenderTargetView> Dx11SwapChain::GetRenderTargetView() const
{
	std::shared_ptr<Dx11RenderTargetView> p_renderTargetView = std::make_shared<Dx11RenderTargetView>();

	p_renderTargetView->m_renderTargetView = m_BackBufferRT;

	return std::reinterpret_pointer_cast<RenderTargetView>(p_renderTargetView);
}
