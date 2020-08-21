

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

#include <Services/QuestService.h>
#include <Services/ImguiService.h>

#include <Games/Skyrim/PlayerCharacter.h>
#include <Games/Skyrim/Forms/TESQuest.h>
#include <Games/Skyrim/FormManager.h>
#include <imgui.h>

#include <Messages/RequestQuestUpdate.h>

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) : m_world(aWorld)
{
    m_joinedConnection = aDispatcher.sink<ConnectedEvent>().connect<&QuestService::OnConnected>(this);
    m_leftConnection = aDispatcher.sink<DisconnectedEvent>().connect<&QuestService::OnDisconnected>(this);
    m_drawConnection = aImguiService.OnDraw.connect<&QuestService::OnDraw>(this);
}

QuestService::~QuestService()
{
}

//void __fastcall ToggleQuestActiveStatus(__int64 a1)

void QuestService::OnConnected(const ConnectedEvent&) noexcept
{
    // About the Events:
    // TESQuestStageItemDoneEvent gets fired to late, we instead use TESQuestStageEvent, because it responds immediately.
    // TESQuestInitEvent can be instead managed by start stop quest management.

    // bind game event listeners
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStartStopEvent.RegisterSink(this);
    pEventList->questStageEvent.RegisterSink(this);

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

static void DoStartQuest(uint32_t formId)
{
    auto searchRegistry = [](uint32_t formId) -> TESQuest*
    {
        auto& questRegistry = FormManager::Get()->quests;

        // this may use bsearch
        auto it = std::find_if(questRegistry.begin(), questRegistry.end(), [formId](TESQuest* it) { 
            return it->formID == formId; 
        });

        return it != questRegistry.end() ? *it : nullptr;     
    };

    if (auto* pQuest = searchRegistry(formId))
    {
        pQuest->scopedStatus = -1;
        pQuest->flags |= TESQuest::Flags::Enabled;
    
        //TODO: set quest starter
    }
}

void QuestService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // remove quest listener
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStageEvent.UnRegisterSink(this);
    pEventList->questStartStopEvent.UnRegisterSink(this);

    m_questCount = PlayerCharacter::Get()->questTargets.length;
}

bool QuestService::IsNonSyncableQuest(TESQuest* apQuest)
{
    // non story quests are "blocked" and not synced
    auto& stages = apQuest->stages;
    return apQuest->type == 0 // internal event
        || apQuest->type == 6 || stages.empty();
}

BSTEventResult QuestService::OnEvent(const TESQuestStageEvent* apEvent, const EventDispatcher<TESQuestStageEvent>*)
{
    if (auto* pQuest = static_cast<TESQuest*>(TESForm::GetById(apEvent->formId)))
    {
        if (IsNonSyncableQuest(pQuest))
            return BSTEventResult::kOk;

        GameId Id;
        auto& modSys = m_world.GetModSystem();
        if (!modSys.GetServerModId(pQuest->formID, Id))
            return BSTEventResult::kOk;

        RequestQuestUpdate update;
        update.Id = Id;
        update.Stage = apEvent->stageId;
        update.Status = RequestQuestUpdate::StageUpdate;

        m_world.GetTransport().Send(update);
    }

    return BSTEventResult::kOk;
}

BSTEventResult QuestService::OnEvent(const TESQuestStartStopEvent* apEvent, const EventDispatcher<TESQuestStartStopEvent>*)
{
    if (auto* pQuest = static_cast<TESQuest*>(TESForm::GetById(apEvent->formId)))
    {
        if (IsNonSyncableQuest(pQuest))
            return BSTEventResult::kOk;

        GameId Id;
        auto& modSys = m_world.GetModSystem();
        if (!modSys.GetServerModId(pQuest->formID, Id))
            return BSTEventResult::kOk;

        RequestQuestUpdate update;
        update.Id = Id;
        update.Stage = pQuest->currentStage;
        update.Status = pQuest->IsStopped() ? RequestQuestUpdate::Stopped : RequestQuestUpdate::Started;

        m_world.GetTransport().Send(update);
    }

    return BSTEventResult::kOk;
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
