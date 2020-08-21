#pragma once

#include <Scripts/EntityHandle.h>

namespace Script
{
    struct Quest;

    struct Player : EntityHandle
    {
        Player(entt::entity aEntity, World& aWorld);

        const Vector<String>& GetMods() const;
        const String& GetIp() const;
        const uint64_t GetDiscordId() const;

        [[nodiscard]] const Vector3<float>& GetPosition() const;
        [[nodiscard]] const Vector3<float>& GetRotation() const;
        [[nodiscard]] float GetSpeed() const;

        Player& operator=(const Player& acRhs)
        {
            EntityHandle::operator=(acRhs);

            return *this;
        }

        bool AddComponent(sol::object aObject) const override;

        const Quest& AddQuest(String& aModName, uint16_t aformId);

        bool RemoveQuest(uint16_t aformId);

        const Vector<Quest>& GetQuests() const;
    };
}
