

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

#include <Services/QuestService.h>
#include <Services/ImguiService.h>

#include <Games/Misc/QuestCallbackManager.h>
#include <Games/Skyrim/PlayerCharacter.h>
#include <Games/Skyrim/Forms/TESQuest.h>
#include <Games/Skyrim/FormManager.h>
#include <Games/Fallout4/PlayerCharacter.h>
#include <Games/Fallout4/Forms/TESQuest.h>
#include <Games/Fallout4/FormManager.h>
#include <Games/Fallout4/Events/EventDispatcher.h>

#include <imgui.h>

#include <Messages/RequestQuestUpdate.h>
#include <Messages/NotifyQuestUpdate.h>

static TESQuest* FindQuestByNameId(const String &name)
{
    auto& questRegistry = FormManager::Get()->quests;
    auto it = std::find_if(questRegistry.begin(), questRegistry.end(), [name](auto* it) 
    { 
         return std::strcmp(it->idName.AsAscii(), name.c_str()); 
    });

    return it != questRegistry.end() ? *it : nullptr;
}

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) : m_world(aWorld)
{
    m_joinedConnection = aDispatcher.sink<ConnectedEvent>().connect<&QuestService::OnConnected>(this);
    m_leftConnection = aDispatcher.sink<DisconnectedEvent>().connect<&QuestService::OnDisconnected>(this);
    m_drawConnection = aImguiService.OnDraw.connect<&QuestService::OnDraw>(this);
    m_questUpdateConnection = aDispatcher.sink<NotifyQuestUpdate>().connect<&QuestService::OnQuestUpdate>(this);
}

//Idea: ToggleQuestActiveStatus(__int64 a1)

void QuestService::OnConnected(const ConnectedEvent&) noexcept
{
    // A note about the Gameevents:
    // TESQuestStageItemDoneEvent gets fired to late, we instead use TESQuestStageEvent, because it responds immediately.
    // TESQuestInitEvent can be instead managed by start stop quest management.
#if TP_FALLOUT
    GetEventDispatcher_TESQuestStartStopEvent()->RegisterSink(this);
    GetEventDispatcher_TESQuestStageEvent()->RegisterSink(this);
#else
    // bind game event listeners
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStartStopEvent.RegisterSink(this);
    pEventList->questStageEvent.RegisterSink(this);
#endif

    // deselect any active quests
    auto* pPlayer = PlayerCharacter::Get();
    for (auto& objective : pPlayer->objectives)
    {
        auto* pQuest = objective.instance->quest;
        pQuest->SetActive(false);
    }
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
    TESQuest* pQuest = static_cast<TESQuest*>(TESForm::GetById(aFormId));

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
            pQuest->SetStage(aStage);
            return pQuest;
        }
    }

    return nullptr;
}

bool QuestService::StopQuest(uint32_t aformId)
{
    auto* pQuest = static_cast<TESQuest*>(TESForm::GetById(aformId));
    pQuest->SetActive(false);
    pQuest->SetStopped();

    return true;
}

void QuestService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // remove quest listener
#if TP_FALLOUT
    GetEventDispatcher_TESQuestStartStopEvent()->UnRegisterSink(this);
    GetEventDispatcher_TESQuestStageEvent()->UnRegisterSink(this);
#else
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStageEvent.UnRegisterSink(this);
    pEventList->questStartStopEvent.UnRegisterSink(this);
#endif
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
    // there is no reason to even fetch the quest object, since the event provides everything already....
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

void QuestService::DebugDumpQuests()
{
    auto& quests = FormManager::Get()->quests;
    for (auto* it : quests)
    {
        std::printf("%x|%d|%d|%s\n", it->formID, it->type, it->priority, it->idName.AsAscii());
    }
}

void QuestService::OnDraw() noexcept
{
    auto* pPlayer = PlayerCharacter::Get();
    if (!pPlayer) return;

    #if 1
    ImGui::Begin("QuestLog");

    for (auto &it : pPlayer->objectives)
    {
        auto* pQuest = it.instance->quest;
        if (pQuest->IsActive())
        {
            ImGui::TextColored({255.f, 0.f, 255.f, 255.f}, "%s|%x @ %p", pQuest->idName.AsAscii(),
                               pQuest->flags, (void*)pQuest);

            for (auto* it : pQuest->stages)
            {
                ImGui::TextColored({0.f, 255.f, 255.f, 255.f}, "Stage: %d|%x", it->stageIndex, it->flags);
            }
        }
        else
        {
            ImGui::Text("%s|%d|%x", pQuest->idName.AsAscii(), pQuest->IsActive(), pQuest->flags);
        }
    }

    ImGui::End();
    #endif
}
