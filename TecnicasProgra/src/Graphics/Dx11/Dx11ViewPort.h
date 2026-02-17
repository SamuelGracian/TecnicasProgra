#pragma once
#include <d3d11.h>

#include"Graphics/ViewPort.h"

class Dx11ViewPort : public ViewPort
{
public:
	Dx11ViewPort() :
		m_viewPort(nullptr)
	{ }

	~Dx11ViewPort() = default;

	D3D11_VIEWPORT* m_viewPort;
	
};