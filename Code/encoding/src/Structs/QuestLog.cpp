
#include <Structs/QuestLog.h>

#include <Serialization.hpp>
#include <Structs/Mods.h>
#include <algorithm>

using TiltedPhoques::Serialization;

bool QuestLog::operator==(const QuestLog& acRhs) const noexcept
{
    return Entries == acRhs.Entries;
}

bool QuestLog::operator!=(const QuestLog& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void QuestLog::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Entries.size());
    for (const auto& e : Entries)
    {
        e.Serialize(aWriter);
    }
}

void QuestLog::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    const auto count = Serialization::ReadVarInt(aReader);
    Entries.resize(count);

    for (auto& e : Entries)
    {
        e.Deserialize(aReader);
    }
}
