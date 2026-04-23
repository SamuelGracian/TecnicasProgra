#pragma once
#include <d3d11.h>
#include "Graphics/SamplerState.h"

class Dx11SamplerState : public SamplerState
{
public:

	Dx11SamplerState() : m_samplerState(nullptr){}

	~Dx11SamplerState()
	{
		if (m_samplerState) m_samplerState->Release();
	}

	ID3D11SamplerState* m_samplerState;

};