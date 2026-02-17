#pragma once 

class RenderTargetView;

class SwapChain
{
    public:
    SwapChain() = default;
    ~SwapChain() = default;

    virtual void Present(uint32_t SyncInterval, uint32_t Flags) = 0;

    virtual std::shared_ptr<RenderTargetView> GetRenderTargetView() const = 0;
};