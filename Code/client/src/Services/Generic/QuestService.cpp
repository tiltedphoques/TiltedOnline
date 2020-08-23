

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

#include <Services/QuestService.h>
#include <Services/ImguiService.h>

#include <Games/Skyrim/PlayerCharacter.h>
#include <Games/Skyrim/Forms/TESQuest.h>
#include <Games/Skyrim/FormManager.h>
#include <Games/Skyrim/Misc/QuestCallbackManager.h>
#include <imgui.h>

#include <Messages/RequestQuestUpdate.h>
#include <Messages/NotifyQuestUpdate.h>

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) : m_world(aWorld)
{
    m_joinedConnection = aDispatcher.sink<ConnectedEvent>().connect<&QuestService::OnConnected>(this);
    m_leftConnection = aDispatcher.sink<DisconnectedEvent>().connect<&QuestService::OnDisconnected>(this);
    m_drawConnection = aImguiService.OnDraw.connect<&QuestService::OnDraw>(this);
    m_questUpdateConnection = aDispatcher.sink<NotifyQuestUpdate>().connect<&QuestService::OnQuestUpdate>(this);
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

void QuestService::OnQuestUpdate(const NotifyQuestUpdate& aUpdate) noexcept
{
    bool bResult = false;
    switch (aUpdate.Status)
    {
    case NotifyQuestUpdate::Started: 
    {
        if (auto* pQuest = SetQuestStage(aUpdate.Id.BaseId, 0))
        {
            pQuest->SetActive(true);
            bResult = true;
        }
        break;
    }
    case NotifyQuestUpdate::StageUpdate:
        bResult = SetQuestStage(aUpdate.Id.BaseId, aUpdate.Stage);
        break;
    case NotifyQuestUpdate::Stopped:
        bResult = StopQuest(aUpdate.Id.BaseId);
        break;
    default:
        break;
    }

    if (!bResult)
        spdlog::error("Failed to update the client quest state");
}

TESQuest* QuestService::SetQuestStage(uint32_t aFormId, uint16_t aStage)
{
    // first ask the quest journal for SPEEEEEEEEEEED reasons
    TESQuest* pQuest = PlayerCharacter::Get()->GetQuest(aFormId);
    if (!pQuest)
        pQuest = SearchQuestRegistry(aFormId);
    if (!pQuest)
        return nullptr;

    // force quest update
    pQuest->flags |= TESQuest::Enabled | TESQuest::Started;
    pQuest->scopedStatus = -1;

    bool bNeedsRegistration = false;
    if (pQuest->UnkSetRunning(bNeedsRegistration, false))
    {
        auto* pCallbackMgr = QuestCallbackManager::Get();

        if (bNeedsRegistration)
            pCallbackMgr->RegisterQuest(aFormId);
        else
        {
            //pCallbackMgr->NotifyStage(aFormId, aStage);

            // rollback the quest stage, dont ask me why this is done
            // blame the game
            //if (!pQuest->SetStage(aStage))
            //    pCallbackMgr->ResetStage(aFormId, aStage);
            pQuest->SetStage(aStage);
            return pQuest;
        }
    }

    return nullptr;
}

bool QuestService::StopQuest(uint32_t aformId)
{
    // first ask the quest journal for SPEEEEEEEEEEED reasons
    TESQuest* pQuest = PlayerCharacter::Get()->GetQuest(aformId);
    if (!pQuest)
        pQuest = SearchQuestRegistry(aformId);
    if (!pQuest)
        return false;

    pQuest->SetActive(false);
    pQuest->SetStopped();
    return true;
}

void QuestService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // remove quest listener
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStageEvent.UnRegisterSink(this);
    pEventList->questStartStopEvent.UnRegisterSink(this);

    m_questCount = PlayerCharacter::Get()->questTargets.length;
}

TESQuest* QuestService::SearchQuestRegistry(uint32_t aFormId)
{
    auto& questRegistry = FormManager::Get()->quests;
    auto it = std::find_if(questRegistry.begin(), questRegistry.end(), [aFormId](TESQuest* it) 
    { 
        return it->formID == aFormId; 
    });

    return it != questRegistry.end() ? *it : nullptr;
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

    if (ImGui::Button("lol"))
        SetQuestStage(0x000D517A, 0)->SetActive(true);

    if (ImGui::Button("Finish Civial War"))
        SetQuestStage(0x000D517A, 200)->SetActive(true);

    if (ImGui::Button("Kill Quest"))
        StopQuest(0x000D517A);

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
