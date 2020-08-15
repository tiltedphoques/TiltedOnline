#include <Messages/AuthenticationRequest.h>

void AuthenticationRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, DiscordId);
    Serialization::WriteString(aWriter, Token);
    Mods.Serialize(aWriter);
}

void AuthenticationRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    DiscordId = Serialization::ReadVarInt(aReader);
    Token = Serialization::ReadString(aReader);
    Mods.Deserialize(aReader);
}
