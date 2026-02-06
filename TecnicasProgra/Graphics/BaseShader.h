#pragma once

#include <cstdint>

class BaseShader
{
    public:
    BaseShader();
    ~BaseShader();

    private:
    uint32_t m_ByteCodeLenght;

};