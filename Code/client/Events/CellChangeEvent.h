#pragma once

struct CellChangeEvent
{
    explicit CellChangeEvent(const uint32_t aCellId, const char* aName) 
        : CellId(aCellId)
        , Name(aName)
    {}

    uint32_t CellId;
    const char* Name;
};
