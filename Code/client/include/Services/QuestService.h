#pragma once

#include <World.h>
#include <Games/Skyrim/Events/EventDispatcher.h>

struct ImguiService;
struct QuestInitHandler;
struct QuestStageHandler;
struct QuestStartStopHandler;

class QuestService final
{
public:
    QuestService(World&, entt::dispatcher&, ImguiService&);
    ~QuestService();

private:
    friend struct QuestStageHandler;

    void OnConnected(const ConnectedEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnDraw() noexcept;

    entt::scoped_connection m_joinedConnection;
    entt::scoped_connection m_leftConnection;
    entt::scoped_connection m_drawConnection;

    std::unique_ptr<QuestStageHandler> m_pQuestStageListener;
    std::unique_ptr<QuestStartStopHandler> m_pQuestStopStartListener;

    uint16_t m_questCount = 0;
    World& m_world;
};
