#pragma once

#include <World.h>
#include <Games/Skyrim/Events/EventDispatcher.h>
#include <Games/Events.h>

struct ImguiService;
struct QuestInitHandler;
struct QuestStageHandler;
struct QuestStartStopHandler;
struct NotifyQuestUpdate;

struct TESQuest;

class QuestService final : public 
    BSTEventSink<TESQuestStartStopEvent>, 
    BSTEventSink<TESQuestStageEvent>,
    BSTEventSink<TESQuestStageItemDoneEvent>
{
public:
    QuestService(World&, entt::dispatcher&, ImguiService&);
    ~QuestService();

    static bool IsNonSyncableQuest(TESQuest* apQuest);
    static TESQuest* SearchQuestRegistry(uint32_t aFormid);

private:
    friend struct QuestEventHandler;

    BSTEventResult OnEvent(const TESQuestStartStopEvent*, const EventDispatcher<TESQuestStartStopEvent>*) override;
    BSTEventResult OnEvent(const TESQuestStageEvent*, const EventDispatcher<TESQuestStageEvent>*) override;

    void OnConnected(const ConnectedEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnDraw() noexcept;
    void OnQuestUpdate(const NotifyQuestUpdate&) noexcept;

    TESQuest* SetQuestStage(uint32_t aformId, uint16_t aStage);
    bool StopQuest(uint32_t aformId);

    entt::scoped_connection m_joinedConnection;
    entt::scoped_connection m_leftConnection;
    entt::scoped_connection m_drawConnection;
    entt::scoped_connection m_questUpdateConnection;

    uint32_t m_questCount = 0;
    World& m_world;
};
