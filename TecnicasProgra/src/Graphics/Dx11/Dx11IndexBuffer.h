#pragma once
#include "d3d11.h"

#include "Graphics/IndexBuffer.h"

class Dx11IndexBuffer : public IndexBuffer
{
public:
	Dx11IndexBuffer(): m_buffer(nullptr) { }

	~Dx11IndexBuffer() = default;

	ID3D11Buffer* m_buffer;
};