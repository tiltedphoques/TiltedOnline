#pragma once

#include <cstdint>
#include <Stl.hpp>
#include <Buffer.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct Quest
{
    Quest() = default;
    ~Quest() = default;

    bool operator==(const Quest& acRhs) const noexcept;
    bool operator!=(const Quest& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t Id;
    uint16_t CurrentStage;
    uint16_t Flags;
    String NameId;
};
