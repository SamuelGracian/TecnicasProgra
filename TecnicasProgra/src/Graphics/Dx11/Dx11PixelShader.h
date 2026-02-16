#pragma once
#include <d3d11.h>

#include"Graphics/PixelShader.h"

class Dx11PixelShader : public PixelShader
{
	public:

	Dx11PixelShader() :
		m_shader(nullptr) 
	{ }

	~Dx11PixelShader() = default;

	//private:

	ID3D11PixelShader* m_shader;
};