#pragma once

#include <Scripts/EntityHandle.h>

struct QuestLogComponent;

namespace Script
{
    struct Quest;
    struct Party;

    struct Player : EntityHandle
    {
        Player(entt::entity aEntity, World& aWorld);

        const Vector<String>& GetMods() const;
        const String& GetIp() const;
        const String& GetName() const;
        const uint64_t GetDiscordId() const;

        // Lua doesn't like Vector3s, so I made it be an array instead. Index starts at 1, rather than 0. Player.position[1] is x, Player.position[2] is y, Player.position[3] is z. Not sure why that is though - bable631
        [[nodiscard]] const std::initializer_list<float> GetPositionL() const;

        [[nodiscard]] const Vector3<float>& GetPosition() const;
        [[nodiscard]] const Vector3<float>& GetRotation() const;
        [[nodiscard]] float GetSpeed() const;

        Player& operator=(const Player& acRhs)
        {
            EntityHandle::operator=(acRhs);

            return *this;
        }

        bool AddComponent(sol::object aObject) const override;
        bool HasMod(const std::string& aModName) const noexcept;

        bool RemoveQuest(uint32_t aformId);
        sol::optional<Quest> AddQuest(const std::string aModName, uint32_t aformId);
        sol::optional<Vector<Quest>> GetQuests() const noexcept;
        sol::optional<Party> GetParty() const noexcept;

        inline entt::entity GetEntityHandle() const { return m_entity; }
    };
}
