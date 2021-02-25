#pragma once

#include <Scripts/Player.h>

namespace Script
{
    struct ServerHandle
    {
        void SendChatMessage(Player aplayer, const std::string aMessage);
    };
}
