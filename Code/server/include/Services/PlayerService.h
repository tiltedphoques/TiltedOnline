#pragma once

#include <Events/PacketEvent.h>

struct World;
struct EnterCellRequest;
struct SendChatMessageRequest;

struct PlayerService
{
    PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~PlayerService() noexcept = default;

    TP_NOCOPYMOVE(PlayerService);

protected:

    void HandleCellEnter(const PacketEvent<EnterCellRequest>& acMessage) const noexcept;
    void HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_cellEnterConnection;
    entt::scoped_connection m_chatMessageConnection;
};
