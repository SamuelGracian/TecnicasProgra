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

    /// <summary>
    /// Set default topology as triangle list
    /// </summary>
    Topology()
        :m_type(Type::TriangleList)
    { }

    virtual ~Topology() = default;

    void SetType(Type newType) {m_type = newType; }

    Type GetType() const { return m_type; }

protected:
    Type m_type;
};