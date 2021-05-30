#pragma once

#include <TiltedCore/Stl.hpp>
#include "Message.h"
#include "MetaMessage.h"

using TiltedPhoques::UniquePtr;

struct ServerAdminMessageFactory
{
    UniquePtr<ServerAdminMessage> Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept;

    template <class T> static auto Visit(T&& func)
    {
        auto s_visitor =
            CreateMessageVisitor<>;

        return s_visitor(std::forward<T>(func));
    }
};