#pragma once
#include <cstdint>
#include <cassert>

class BaseBuffer
{
    public:
    BaseBuffer() = default;
    ~BaseBuffer() = default;

    void SetByteWidth(const uint32_t bytewidth = 0)
    { 
        assert(bytewidth != 0);
        m_byteWidth = bytewidth; 
    }

    const uint32_t GetByteWidth()const { return m_byteWidth; }

    protected:
    uint32_t m_byteWidth;
};