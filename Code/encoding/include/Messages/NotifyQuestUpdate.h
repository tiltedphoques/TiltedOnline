#pragma once

#include "Message.h"
#include <Buffer.hpp>

using TiltedPhoques::Map;

struct NotifyQuestUpdate final : ServerMessage
{
    NotifyQuestUpdate() : ServerMessage(kNotifyQuestUpdate)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

#if 0
    bool operator==(const NotifyQuestUpdate& acRhs) const noexcept
    {
        //return Change == acRhs.Change && GetOpcode() == acRhs.GetOpcode();
    }
#endif
    //Quest Change;
};
