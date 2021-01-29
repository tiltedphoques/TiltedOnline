#pragma once

#include "Message.h"

struct NotifyDamageEvent final : ServerMessage
{
    NotifyDamageEvent() : ServerMessage(kNotifyDamageEvent)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDamageEvent& acRhs) const noexcept
    {
        return m_Id == acRhs.m_Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_Id;
    float m_Damage;
};