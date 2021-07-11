#pragma once

#include <TiltedCore/Filesystem.hpp>

using namespace TiltedPhoques;

namespace fs = std::filesystem;

struct Title
{
    // keep this in sync with the title map!
    enum class Id
    {
        kUnknown,    //< Invalid
        kSkyrimSE,   //< Skyrim Special Edition
        kSkyrimVR,   //< Skyrim VR
        kFallout4,   //< Fallout 4
        kFallout4VR, //< Fallout 4 VR
        kReserved1,  //< ???
        kReserved2,  //< ??? <(-_-)>
        kCount
    };

    uint32_t steamAppId;
    const wchar_t* fullGameName;
    const char* shortGameName;
    const wchar_t* tpClientName;

    static const Title* ToTitle(Title::Id aId) noexcept;
    static const Title* ToTitle(const char* acAlias) noexcept;
    static const Id ToTitleId(const Title& acTitle) noexcept;
};

bool FindTitlePath(const Title& aTitle, bool aForceReselect, fs::path& aTitlePath, fs::path& aExePath);
