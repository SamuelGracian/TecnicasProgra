#pragma once 
#include <d3d11.h>

#include "Graphics/Topology.h"

class Dx11Topology : public Topology
{

public:
	Dx11Topology();

	~Dx11Topology() = default;

	D3D11_PRIMITIVE_TOPOLOGY GetDx11Topology() const { return m_topology; }

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
};