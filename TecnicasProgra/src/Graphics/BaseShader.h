#pragma once

#include <cstdint>

class BaseShader
{
    public:
    BaseShader() = default;
    ~BaseShader() = default;

    protected:
    uint32_t m_ByteCodeLenght;

};