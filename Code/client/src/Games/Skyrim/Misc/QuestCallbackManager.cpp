
#if TP_SKYRIM

#include <Games/Skyrim/Misc/QuestCallbackManager.h>

QuestCallbackManager* QuestCallbackManager::Get() noexcept
{
    POINTER_SKYRIMSE(QuestCallbackManager*, s_instance, 0x142F4F3B0 - 0x140000000);
    return *s_instance.Get();
}

void QuestCallbackManager::RegisterQuest(uint32_t formId)
{
    using TRegisterQuest = void(QuestCallbackManager*, uint32_t);
    POINTER_SKYRIMSE(TRegisterQuest, RegisterQuest, 0x1409124E0 - 0x140000000);

    RegisterQuest(this, formId);
}

void QuestCallbackManager::NotifyStage(uint32_t formId, uint16_t stage)
{
    using TNotifyQuestStage = void(QuestCallbackManager*, uint32_t, uint16_t);
    POINTER_SKYRIMSE(TNotifyQuestStage, NotifyQuestStage, 0x140910ED0 - 0x140000000);

    NotifyQuestStage(this, formId, stage);
}

void QuestCallbackManager::ResetStage(uint32_t formId, uint16_t stage)
{
    using TResetStage = void(QuestCallbackManager*, uint32_t, uint16_t);
    POINTER_SKYRIMSE(TResetStage, ResetStage, 0x140911180 - 0x140000000);

    ResetStage(this, formId, stage);
}

#endif
