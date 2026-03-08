#pragma once

class DepthStencilView
{
public:

    enum ClearFlags  : uint8_t
    {
        Depth = 1,
        Stencil = 2
    };

    DepthStencilView() = default;
    ~DepthStencilView() = default;

};