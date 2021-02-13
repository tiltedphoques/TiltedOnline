#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::Map;
using TiltedPhoques::String;

struct SendChatMessageRequest final : ClientMessage
{
    SendChatMessageRequest() : ClientMessage(kSendChatMessageRequest)
    {
    }

    virtual ~SendChatMessageRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const SendChatMessageRequest& acRhs) const noexcept
    {
        return ChatMessage == acRhs.ChatMessage && GetOpcode() == acRhs.GetOpcode();
    }

    TiltedPhoques::String ChatMessage;
};
