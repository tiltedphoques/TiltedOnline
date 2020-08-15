#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Stl.hpp>
#include <Structs/Quest.h>

struct RequestQuestUpdate final : ClientMessage
{
    RequestQuestUpdate() : ClientMessage(kRequestQuestUpdate)
    {
    }

    virtual ~RequestQuestUpdate() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestQuestUpdate& acRhs) const noexcept
    {
        return Change == acRhs.Change && GetOpcode() == acRhs.GetOpcode();
    }

    Quest Change;
};
