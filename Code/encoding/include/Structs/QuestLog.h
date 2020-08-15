#pragma once

#include <Structs/Quest.h>

using TiltedPhoques::Vector;

struct QuestLog
{
    Vector<Quest> Entries{};

    QuestLog() = default;
    ~QuestLog() = default;

    bool operator==(const QuestLog& acRhs) const noexcept;
    bool operator!=(const QuestLog& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
