#include <Scripts/Player.h>
#include <Scripts/Npc.h>
#include <Scripts/Quest.h>

#include <World.h>
#include <Components.h>

namespace Script
{
    Player::Player(entt::entity aEntity, World& aWorld)
        : EntityHandle(aEntity, aWorld)
    {}

    const Vector<String>& Player::GetMods() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.Mods;
    }

    const String& Player::GetIp() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.Endpoint;
    }

    const uint64_t Player::GetDiscordId() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.DiscordId;
    }

    const Vector3<float>& Player::GetPosition() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Position;
    }

    const Vector3<float>& Player::GetRotation() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Rotation;
    }

    float Player::GetSpeed() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

#if TP_SKYRIM
        return movementComponent.Variables.Floats[AnimationData::FloatVariables::kSpeed];
#else
        return 0.f;
#endif
    }

    bool Player::AddComponent(sol::object aObject) const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);

        // If the player has a character we add the component to the child, makes no sense to add to a player
        if(playerComponent.Character)
        {
            const Script::Npc npc(*playerComponent.Character, *m_pWorld);
            return npc.AddComponent(aObject);
        }

        return false;
    }

    const Quest& Player::AddQuest(String& aModName, uint16_t aformId)
    {
        auto& questComponent = m_pWorld->get<QuestLogComponent>(m_entity);
        auto& modComponent = m_pWorld->get<ModsComponent>(m_entity);
        auto& entries = questComponent.QuestContent.Entries;
#if 0
        auto it = std::find_if()

        auto& entries = component.QuestContent.Entries;
        auto& service = m_pWorld->GetQuestService();
#endif

        // these are special unitialized flags
        auto &newQuest = entries.emplace_back();

#if 0
        newQuest.Id = aformId;
        newQuest.Flags = -1;
        newQuest.CurrentStage = 0;
#endif

        return reinterpret_cast<Script::Quest&>(newQuest);
    }

    bool Player::RemoveQuest(uint16_t aformId)
    {
        auto& component = m_pWorld->get<QuestLogComponent>(m_entity);
        auto& service = m_pWorld->GetQuestService();


        return false;
    }

    const Vector<Quest>& Player::GetQuests() const
    {
        auto& component = m_pWorld->get<QuestLogComponent>(m_entity);
        return reinterpret_cast<Vector<Script::Quest>&>(component.QuestContent.Entries);
    }
}
