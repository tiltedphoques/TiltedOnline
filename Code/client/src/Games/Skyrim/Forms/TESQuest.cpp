
#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESQuest.h>

TESQuest::State TESQuest::getState()
{
    if (flags >= 0)
    {
        if (unkFlags)
            return State::WaitingPromotion;
        else if (flags & 1)
            return State::Running;
        else
            return State::Stopped;
    }

    return State::WaitingForStage;
}

void TESQuest::SetCompleted(bool force)
{
    using TSetCompleted = void(TESQuest*, bool);
    POINTER_SKYRIMSE(TSetCompleted, SetCompleted, 0x140370290 - 0x140000000);

    SetCompleted(this, force);
}

void TESQuest::CompleteAllObjectives()
{
    using TCompleteAllObjectives = void(TESQuest*);
    POINTER_SKYRIMSE(TCompleteAllObjectives, CompleteAll, 0x1403232E0 - 0x140000000);

    CompleteAll(this);
}

void TESQuest::SetActive(bool toggle)
{
    if (toggle)
        flags |= 0x800;
    else
        flags &= 0xF7FF;
}

bool TESQuest::IsStageDone(uint16_t stageIndex)
{
    for (auto* it : stages)
    {
        if (it->stageIndex == stageIndex)
            return it->IsDone();
    }

    return false;
}

bool TESQuest::Kill()
{
    using TSetStopped = void(TESQuest*, bool);
    POINTER_SKYRIMSE(TSetStopped, SetStopped, 0x14036FFE0 - 0x140000000);

    if (flags & Flags::Enabled)
    {
        unkFlags = 0;
        flags = 0;
        MarkChanged(2);

        //SetStopped(this, false);
        return true;
    }

    return false;
}

static TiltedPhoques::Initializer s_questInitHooks([]() {
    // kill quest init in cold blood
    //TiltedPhoques::Write<uint8_t>(0x140370910 - 0x140000000, 0xC3);
});


#endif
