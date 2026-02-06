#pragma once

#include <cstdint>

class Topology
{
    public:

    enum class Type : uint32_t
    {
        TriangleList = 0,
        TriangleStrip,
        LineList,
        PointList
    };

    Topology();
    ~Topology();

};