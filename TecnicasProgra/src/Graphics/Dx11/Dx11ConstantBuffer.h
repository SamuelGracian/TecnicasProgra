#pragma once
#include <d3d11.h>

#include "Graphics/ConstantBuffer.h"

class Dx11ConstantBuffer : public ConstantBuffer
{
public:
	Dx11ConstantBuffer()
		: m_buffer(nullptr)
	{ }

	~Dx11ConstantBuffer() = default;

	ID3D11Buffer* Getbuffer();
//private:
	ID3D11Buffer* m_buffer;
};