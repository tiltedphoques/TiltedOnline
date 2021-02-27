#include <Components.h>
#include <GameServer.h>

#include <Scripts/Player.h>
#include <Services/ChatService.h>

#include <Messages/SendChatMessageRequest.h>
#include <Messages/NotifyChatMessageBroadcast.h>

#include <regex>;

ChatService::ChatService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
    , m_chatMessageConnection(
          aDispatcher.sink<PacketEvent<SendChatMessageRequest>>().connect<&ChatService::HandleChatMessage>(this))
{
}

void ChatService::SendChatMessage(Script::Player aPlayer, const std::string aMessage)
{
    spdlog::debug(aMessage);
    NotifyChatMessageBroadcast notifyMessage;
    notifyMessage.PlayerName = "";
    notifyMessage.ChatMessage = aMessage;

    auto& playerComponent = m_world.get<PlayerComponent>(aPlayer.GetEntityHandle());
    GameServer::Get()->Send(playerComponent.ConnectionId, notifyMessage);
}

void ChatService::BroadcastMessage(const std::string aMessage)
{
    NotifyChatMessageBroadcast notifyMessage;
    notifyMessage.PlayerName = "";
    notifyMessage.ChatMessage = aMessage;

    auto playerView = m_world.view<PlayerComponent>();
    for (auto entity : playerView)
    {
        auto& player = playerView.get<PlayerComponent>(entity);
        GameServer::Get()->Send(player.ConnectionId, notifyMessage);
    }
}

void ChatService::HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept
{
    auto playerView = m_world.view<PlayerComponent>();

    const auto itor = std::find_if(std::begin(playerView), std::end(playerView),
                                   [playerView, connectionId = acMessage.ConnectionId](auto entity) {
                                       return playerView.get(entity).ConnectionId == connectionId;
                                   });

    if (itor == std::end(playerView))
    {
        spdlog::error("Connection {:x} is not associated with a player.", acMessage.ConnectionId);
        return;
    }

    auto& playerComponent = playerView.get(*itor);

    NotifyChatMessageBroadcast notifyMessage;
    notifyMessage.PlayerName = playerComponent.Username;

    std::regex escapeHtml{"<[^>]+>\s+(?=<)|<[^>]+>"};
    notifyMessage.ChatMessage = std::regex_replace(acMessage.Packet.ChatMessage, escapeHtml, "");

    //if (notifyMessage.ChatMessage)
    //TODO Fix

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        spdlog::debug("Sending message from Server to client: " + notifyMessage.ChatMessage + " - " +
                      notifyMessage.PlayerName);
        auto& player = view.get<PlayerComponent>(entity);
        GameServer::Get()->Send(player.ConnectionId, notifyMessage);
    }
}
