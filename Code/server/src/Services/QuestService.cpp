
#include <World.h>
#include <Services/QuestService.h>
#include <Messages/RequestQuestUpdate.h>

#include <Components.h>

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

    auto& changeQuest = acMessage.Packet.Change;

    if (viewIt == view.end())
    {
        spdlog::error("Quest {:x} is not associated with connection {:x}", changeQuest.Id, acMessage.ConnectionId);
        return;
    }

    auto& questComponent = view.get<QuestLogComponent>(*viewIt);
    auto& entries = questComponent.QuestContent.Entries;

    auto questIt = std::find_if(entries.begin(), entries.end(),
                               [&changeQuest](const Quest &e) { return e.Id == changeQuest.Id;
                               });

    if (questIt == entries.end())
    {
        spdlog::info("Quest: {}:{:x}", changeQuest.NameId, changeQuest.Id);
        entries.emplace_back(changeQuest);
    }
    else
    {
        auto& record = *questIt;
        record.Flags = changeQuest.Flags;
        record.CurrentStage = changeQuest.CurrentStage;
        std::printf("Editing quest record %s\n", changeQuest.NameId.c_str());
    }
}
