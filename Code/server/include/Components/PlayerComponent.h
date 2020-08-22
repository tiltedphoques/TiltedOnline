#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct PlayerComponent
{
    PlayerComponent(const ConnectionId_t aConnectionId)
        : ConnectionId(aConnectionId)
    {}

    ConnectionId_t ConnectionId;
    std::optional<entt::entity> Character;
    std::optional<entt::entity> PlayerHandle;
    Vector<String> Mods;
    Vector<uint16_t> ModIds;
    uint64_t DiscordId;
    String Endpoint;
};
