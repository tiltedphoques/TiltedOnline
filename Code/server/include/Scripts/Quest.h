#pragma once

namespace Script
{
    struct Quest
    {
        inline uint16_t GetId() const
        {
            return 0;
        }

        inline uint16_t GetStage() const
        {
            return 0;
            //return CurrentStage;
        }

        void SetStage(uint16_t aNewStage)
        {
            //CurrentStage = aNewStage;
        }
    };
}
