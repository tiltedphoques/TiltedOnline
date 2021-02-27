#pragma once
#include <Scripts/Player.h>

#include <Messages/SendChatMessageRequest.h>

struct World;

class ChatService
{
  public:
    ChatService(World& aWorld, entt::dispatcher& aDispatcher);

    void SendChatMessage(Script::Player aPlayer, const std::string aMessage);
    void BroadcastMessage(const std::string aMessage);

  protected:
    void HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept;

  private:
    World &m_world;

    entt::scoped_connection m_chatMessageConnection;
};
