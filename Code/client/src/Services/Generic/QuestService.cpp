

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

#include <Services/QuestService.h>
#include <Services/ImguiService.h>

#include <Games/Skyrim/PlayerCharacter.h>
#include <Games/Skyrim/Forms/TESQuest.h>
#include <imgui.h>

#include <Messages/RequestQuestUpdate.h>

struct QuestStageHandler final : public BSTEventSink<TESQuestStageEvent>
{
    QuestStageHandler(QuestService& aQuestSv) : m_parent(aQuestSv) {}
    ~QuestStageHandler() override{}

    Result OnEvent(const TESQuestStageEvent* apEvent, const EventDispatcher<TESQuestStageEvent>*) override
    {
        if (auto* pQuest = static_cast<TESQuest*>(TESForm::GetById(apEvent->formId)))
        {
            auto& stages = pQuest->stages;

            // cant really do anything about it
            if (stages.empty())
                return Result::kOk;

            // TODO: what is this callback?

            // keep in mind: a quest can start and stop at the same time
            // if it only has one stage!
            auto* stageNode = &stages.entry;
            bool questCompleted = false;
            bool questStarting = false;

            const auto* firstNode = stageNode;

            if (firstNode->data->stageIndex == apEvent->stage)
                questStarting = true;

            while (stageNode->next != nullptr)
                stageNode = stageNode->next;

            if (firstNode == stageNode || stageNode->data->stageIndex == apEvent->stage)
                questCompleted = true;

            if (questCompleted)
            {
                std::printf("Quest %s completed | type %d\n", pQuest->GetName(), pQuest->type);
            }

            if (questStarting)
            {
                std::printf("Quest %s started | type %d\n", pQuest->GetName(), pQuest->type);
            }

            // this is kinda badly done... an update shouldnd contain a name etc.
            RequestQuestUpdate update;
            
            auto& quest = update.Change;
            quest.CurrentStage = apEvent->stage;
            quest.Id = pQuest->formID;
            quest.Flags = pQuest->flags;
            quest.NameId = pQuest->idName.AsAscii();

            m_parent.m_world.GetTransport().Send(update);
            m_pLastQuest = pQuest;
        }

        return Result::kOk;
    }

private:
    QuestService& m_parent;
    TESQuest* m_pLastQuest = nullptr;
};

struct QuestStartStopHandler final : public BSTEventSink<TESQuestStartStopEvent>
{
    QuestStartStopHandler(QuestService& aQuestSv) : m_parent(aQuestSv) {}
    ~QuestStartStopHandler() override {}

    // we need this handler in order to respond 
    // to branching quests...
    Result OnEvent(const TESQuestStartStopEvent* apEvent, const EventDispatcher<TESQuestStartStopEvent>*) override
    {
        auto* pQuest = static_cast<TESQuest*>(TESForm::GetById(apEvent->formId));
        if (pQuest)
        {
            std::printf("%s quest %s\n", pQuest->IsStopped() ? "stopped" : "started" , pQuest->idName.AsAscii());

        }


        return Result::kOk;
    }

private:
    QuestService& m_parent;
};


QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) : m_world(aWorld)
{
    m_joinedConnection = aDispatcher.sink<ConnectedEvent>().connect<&QuestService::OnConnected>(this);
    m_leftConnection = aDispatcher.sink<DisconnectedEvent>().connect<&QuestService::OnDisconnected>(this);
    m_drawConnection = aImguiService.OnDraw.connect<&QuestService::OnDraw>(this);

    m_pQuestStageListener = std::make_unique<QuestStageHandler>(*this);
    m_pQuestStopStartListener = std::make_unique<QuestStartStopHandler>(*this);
}

QuestService::~QuestService()
{
}

void QuestService::OnConnected(const ConnectedEvent&) noexcept
{
    // bind game event listeners
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStageEvent.RegisterSink(m_pQuestStageListener.get());
    pEventList->questStartStopEvent.RegisterSink(m_pQuestStopStartListener.get());

    // deselect any active quests
    auto* pPlayer = PlayerCharacter::Get();
    auto& questTargets = pPlayer->questTargets;

    for (auto i = 0; i < (questTargets.length * 2); i += 2)
    {
        auto* pQuest = questTargets[i]->quest;
        pQuest->SetActive(false);
    }

    m_questCount = questTargets.length;
}

void QuestService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // remove quest listener
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStageEvent.UnRegisterSink(m_pQuestStageListener.get());
    pEventList->questStartStopEvent.UnRegisterSink(m_pQuestStopStartListener.get());

    m_questCount = PlayerCharacter::Get()->questTargets.length;
}

void QuestService::OnDraw() noexcept
{
    auto* pPlayer = PlayerCharacter::Get();
    if (!pPlayer) return;

    ImGui::Begin("Player Quest Log");
    for (auto i = 0; i < (pPlayer->questTargets.length * 2); i += 2)
    {
        auto* pObjective = pPlayer->questTargets[i];
        auto* pQuest = pObjective->quest;
        
        if (pQuest->IsActive())
        {
            ImGui::TextColored({255.f, 0.f, 255.f, 255.f}, "%s|%d|%x @ %p", pQuest->idName.AsAscii(), pQuest->IsActive(),
                        pQuest->flags, (void*)pQuest);

            for (auto* it : pQuest->stages)
            {
                ImGui::TextColored({0.f, 255.f, 255.f, 255.f}, "Stage: %d|%x", it->stageIndex, it->flags);
            
            }
        }
        else
        {
            ImGui::Text("%s|%d|%x",  pQuest->idName.AsAscii(), pQuest->IsActive(),
                        pQuest->flags);
        }
    }
    ImGui::End();
}
