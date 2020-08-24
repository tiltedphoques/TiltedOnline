#if defined(TP_FALLOUT)

#include <Games/Fallout4/Forms/TESQuest.h>
#include <Games/Fallout4/PlayerCharacter.h>

TESQuest* PlayerCharacter::GetQuest(uint32_t aFormId) const noexcept
{
    for (size_t i = 0; i < objectives.length; i++)
    {
        auto* pQuest = objectives[i].instance->quest;
        if (pQuest->formID == aFormId)
            return pQuest;
    }

    return nullptr;
}

#endif
