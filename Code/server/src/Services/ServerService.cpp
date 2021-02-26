#include <Components.h>
#include <GameServer.h>

#include <Messages/NotifyChatMessageBroadcast.h>

ServerService::ServerService(World &aWorld, entt::dispatcher &aDispatcher) : m_world(aWorld)
{
}

uint32_t ServerService::GetId() const noexcept
{
    return 1337;
}

void ServerService::SendChatMessage(const std::string aMessage)
{
    spdlog::debug(aMessage);
    //NotifyChatMessageBroadcast notifyMessage;
    //notifyMessage.PlayerName = "";
    //notifyMessage.ChatMessage = aMessage;

    //auto& playerComponent = m_pWorld->get<PlayerComponent>(aPlayer.GetEntityHandle());
    //GameServer::Get()->Send(playerComponent.ConnectionId, notifyMessage);
}

