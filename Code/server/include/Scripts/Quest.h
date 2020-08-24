#pragma once

#include <Scripts/Player.h>

namespace Script
{
    struct Quest
    {
        explicit Quest(uint32_t aId, uint16_t aStage, World& aWorld) : 
            m_id(aId), m_stage(aStage), m_pWorld(&aWorld)
        {}

        [[nodiscard]] uint32_t GetId() const
        {
            return m_id;
        }

        [[nodiscard]] inline uint16_t GetStage() const
        {
            return m_stage;
        }

        void SetStage(uint16_t aNewStage, std::vector<Player> aPlayers);

        //World *

    private:
        World* m_pWorld = nullptr;
        uint32_t m_id = 0;
        uint16_t m_stage = 0;
    };
}
