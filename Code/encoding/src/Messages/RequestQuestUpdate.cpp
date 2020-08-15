
#include <Messages/RequestQuestUpdate.h>
#include <Serialization.hpp>

void RequestQuestUpdate::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Change.Serialize(aWriter);
}

void RequestQuestUpdate::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
    Change.Deserialize(aReader);
}
