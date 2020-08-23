#if defined(TP_SKYRIM)

#include <Games/Skyrim/Forms/TESQuest.h>
#include <Games/Skyrim/PlayerCharacter.h>

TESQuest* PlayerCharacter::GetQuest(uint32_t aFormId) const noexcept
{
    // 16 byte padding ¯\_(ツ)_/¯
    for (auto i = 0; i < (questTargets.length * 2); i += 2)
    {
        auto* pQuest = questTargets[i]->quest;

        if (pQuest->formID == aFormId)
            return pQuest;
    }

    return nullptr;
}

#endif
