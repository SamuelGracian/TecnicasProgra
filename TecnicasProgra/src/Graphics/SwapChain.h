#pragma once 
#include "Graphics/RenderTargetView.h"

class SwapChain
{
    public:
    SwapChain() = default;
    ~SwapChain() = default;

    virtual void Present(uint32_t SyncInterval, uint32_t Flags) = 0;

    virtual std::shared_ptr<RenderTargetView> GetRenderTargetView() = 0;

protected:
    std::shared_ptr<RenderTargetView> m_renderTargetView;
};