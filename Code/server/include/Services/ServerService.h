#pragma once
#include <Scripts/Player.h>

struct World;

class ServerService
{
  public:
    ServerService(World &aWorld, entt::dispatcher &aDispatcher);

    uint32_t GetId() const noexcept;
    void SendChatMessage(Script::Player aPlayer, const std::string aMessage);

  private:
    World &m_world;
};
