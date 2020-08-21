
#include <GameServer.h>
#include <Components.h>

#include <World.h>
#include <Services/QuestService.h>

#include <Messages/RequestQuestUpdate.h>
#include <Messages/NotifyQuestUpdate.h>

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher) : m_world(aWorld)
{
    m_questUpdateConnection =
        aDispatcher.sink<PacketEvent<RequestQuestUpdate>>().connect<&QuestService::HandleQuestChanges>(this);
}

void QuestService::HandleQuestChanges(const PacketEvent<RequestQuestUpdate>& acMessage) noexcept
{
    auto view = m_world.view<PlayerComponent, QuestLogComponent>();
    auto viewIt = std::find_if(view.begin(), view.end(),
                           [view, connectionId = acMessage.ConnectionId](auto entity) {
        return view.get<PlayerComponent>(entity).ConnectionId == connectionId;
    });

    const auto& message = acMessage.Packet;

    if (viewIt == view.end())
    {
        spdlog::error("Quest {:x} is not associated with connection {:x}", message.Id.BaseId,
                      acMessage.ConnectionId);
        return;
    }

    auto& questComponent = view.get<QuestLogComponent>(*viewIt);
    auto& entries = questComponent.QuestContent.Entries;

    auto questIt = std::find_if(entries.begin(), entries.end(), [&message](const auto& e) { 
        return e.Id == message.Id;
    });

    if (message.Status == RequestQuestUpdate::Started || 
        message.Status == RequestQuestUpdate::StageUpdate)
    {
        // in order to prevent bugs when a quest is in progress
        // and being updated we add it as a new quest record to
        // maintain a proper remote questlog state.
        if (questIt == entries.end())
        {
            auto& newQuest = entries.emplace_back();
            newQuest.Id = message.Id;
            newQuest.Stage = message.Stage;

            if (message.Status == RequestQuestUpdate::Started)
            {
                spdlog::info("Started Quest: {:x}", message.Id.BaseId);

                // we only trigger that on remote quest start
                m_world.GetScriptService().HandleQuestStart(acMessage.ConnectionId);
            }
        } 
        else 
        {
            spdlog::info("Updated quest: {:x}:{}", message.Id.BaseId, message.Stage);

            auto& record = *questIt;
            record.Id = message.Id;
            record.Stage = message.Stage;
        }
    }
    else if (message.Status == RequestQuestUpdate::Stopped)
    {
        spdlog::info("Stopped quest: {:x}", message.Id.BaseId);
        entries.erase(questIt);
    }
}
  
// require mod id
void QuestService::StartQuest(uint16_t id)
{
    NotifyQuestUpdate questMsg;

#if 0
    auto& quest = questMsg.Change;
    quest.Flags = 256;
    //quest.Id = id;
    quest.CurrentStage = 0;

    // massive hack
    GameServer::Get()->SendToLoaded(questMsg);
    #endif
}
