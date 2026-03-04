#include "Graphics/Dx11/Dx11SwapChain.h"
#include"Graphics/Dx11/Dx11RenderTargetView.h"

  
void Dx11SwapChain::Present(uint32_t SyncInterval, uint32_t Flags)
{
	m_swapChain->Present(SyncInterval, Flags);
}

std::shared_ptr<RenderTargetView> Dx11SwapChain::GetRenderTargetView()
{
	if (m_renderTargetView == nullptr)
	{
		m_renderTargetView = std::make_shared<Dx11RenderTargetView>();

		auto p_renderTargetView = std::reinterpret_pointer_cast<Dx11RenderTargetView> (m_renderTargetView);

		p_renderTargetView->m_renderTargetView = m_BackBufferRT;
	}
		return m_renderTargetView;
}
