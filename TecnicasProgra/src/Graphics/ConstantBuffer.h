#pragma once
#include <cassert>

#include "BaseBuffers.h"

#define HIGHER_AVAILABLE_SLOT 8

class ConstantBuffer : public BaseBuffer
{
    public:
    ConstantBuffer() = default;
    ~ConstantBuffer() = default;

    void SetSlot(const uint32_t slot) 
    {
        assert(slot < HIGHER_AVAILABLE_SLOT);
        m_slot = slot;
    }

    uint32_t GetSlot() const { return m_slot; }

private:
    uint32_t m_slot;
    void* m_rawData;
    bool m_isUpdatePending;
};