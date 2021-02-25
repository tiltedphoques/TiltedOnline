#include <Scripts/ServerHandle.h>

#include <World.h>
#include <Components.h>
#include <GameServer.h>

#include <Messages/NotifyChatMessageBroadcast.h>

namespace Script
{
    void ServerHandle::SendChatMessage(Player aPlayer, const std::string aMessage)
    {
        spdlog::debug(aMessage);
        NotifyChatMessageBroadcast notifyMessage;
        notifyMessage.PlayerName = "";
        notifyMessage.ChatMessage = aMessage;

        //auto& playerComponent = m_pWorld->get<PlayerComponent>(aPlayer.GetEntityHandle());
        //GameServer::Get()->Send(playerComponent.ConnectionId, notifyMessage);
    }
}
