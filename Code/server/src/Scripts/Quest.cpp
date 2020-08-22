
#include <Scripts/Quest.h>

namespace Script
{
    void Quest::SetStage(uint16_t anewStage, std::vector<Player>& aPlayers)
    {
    
        for (auto& player : aPlayers)
        {
            auto entity = player.GetEntityHandle();
            
        }

        m_stage = anewStage;

        //TBD
    }
}
