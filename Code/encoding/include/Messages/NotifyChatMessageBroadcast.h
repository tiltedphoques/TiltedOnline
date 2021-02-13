#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::Map;
using TiltedPhoques::String;

struct NotifyChatMessageBroadcast final : ServerMessage
{
    NotifyChatMessageBroadcast() : ServerMessage(kNotifyChatMessageBroadcast)
    {
    }

    virtual ~NotifyChatMessageBroadcast() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyChatMessageBroadcast& acRhs) const noexcept
    {
        return PlayerName == acRhs.PlayerName && ChatMessage == acRhs.ChatMessage && GetOpcode() == acRhs.GetOpcode();
    }

    TiltedPhoques::String PlayerName;
    TiltedPhoques::String ChatMessage;
};
