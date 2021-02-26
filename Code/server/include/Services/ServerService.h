#pragma once

struct World;

class ServerService
{
  public:
    ServerService(World &aWorld, entt::dispatcher &aDispatcher);

    uint32_t ServerService::GetId() const noexcept;
    void SendChatMessage(const std::string aMessage);

  private:
    World &m_world;
};
