#include "Dx11Topology.h"

Dx11Topology::Dx11Topology()
    : m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
    switch (m_type)
    {
    case Topology::Type::TriangleList:
        m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case Topology::Type::TriangleStrip:
        m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    case Topology::Type::LineList:
        m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case Topology::Type::PointList:
        m_topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    default:
        m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    }
}
