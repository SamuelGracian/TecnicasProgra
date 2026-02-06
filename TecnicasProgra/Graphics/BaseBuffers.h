#pragma once
#include <cstdint>

class BaseBuffer
{
    public:
    BaseBuffer();
    ~BaseBuffer();

    private:
    uint32_t m_byteWidth;
};