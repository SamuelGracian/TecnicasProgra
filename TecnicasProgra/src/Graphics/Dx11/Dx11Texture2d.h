#pragma once 
#include "Graphics/Texture2d.h"

class Dx11Texture2D : public Texture2D
{
public:
    Dx11Texture2D() : m_texture(nullptr), m_textureView(nullptr) {}
    ~Dx11Texture2D()
    {
        if (m_textureView) m_textureView->Release();
        if (m_texture) m_texture->Release();
    }

    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
};