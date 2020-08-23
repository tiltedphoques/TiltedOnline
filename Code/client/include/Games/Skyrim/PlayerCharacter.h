#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Actor.h>
#include <Games/Skyrim/Misc/TintMask.h>

struct TESQuest;

struct PlayerCharacter : Actor
{
    static constexpr uint32_t Type = FormType::Character;

    static PlayerCharacter* Get() noexcept;

    const GameArray<TintMask*>& GetTints() const noexcept;

    struct QuestTarget
    {
        BSFixedString name;
        TESQuest* quest;
    };

    uint8_t pad1[0x580 - sizeof(Actor)];
    GameArray<QuestTarget*> questTargets; 
    char pad588[0x538 - 8];
    TESForm *locationForm;
    uint8_t padAC8[0x40];
    //0xAC8
    GameArray<TintMask*> baseTints;
    GameArray<TintMask*>* overlayTints;

    uint8_t padPlayerEnd[0xBE0 - 0xB30];

    TESQuest* GetQuest(uint32_t aFormId) const noexcept;
};

static_assert(offsetof(PlayerCharacter, questTargets) == 0x580);
static_assert(offsetof(PlayerCharacter, locationForm) == 0xAC8);
static_assert(offsetof(PlayerCharacter, baseTints) == 0xB10);
static_assert(offsetof(PlayerCharacter, overlayTints) == 0xB28);
static_assert(sizeof(PlayerCharacter) == 0xBE0);

#endif
