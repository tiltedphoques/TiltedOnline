
#include <Structs/Quest.h>
#include <Serialization.hpp>
#include <algorithm>

using TiltedPhoques::Serialization;

bool Quest::operator==(const Quest& acRhs) const noexcept
{
    return Id == acRhs.Id && CurrentStage == acRhs.CurrentStage && Flags == acRhs.Flags && NameId == acRhs.NameId;
}

bool Quest::operator!=(const Quest& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Quest::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    aWriter.WriteBits(CurrentStage, 16);
    aWriter.WriteBits(Flags, 16);
    Serialization::WriteString(aWriter, NameId);
}

void Quest::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    uint64_t tmp;
    aReader.ReadBits(tmp, 16);
    CurrentStage = tmp & 0xFFFF;

    aReader.ReadBits(tmp, 16);
    Flags = tmp & 0xFFFF;

    NameId = Serialization::ReadString(aReader);
}
