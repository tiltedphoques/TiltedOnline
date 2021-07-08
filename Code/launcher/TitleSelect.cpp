
#include "TitleSelect.h"
#include <Windows.h>
#include <spdlog/spdlog.h>

#include "utils/Error.h"
#include "utils/Registry.h"

constexpr wchar_t kTpRegistryPath[] = LR"(Software\TiltedPhoques\TiltedOnline\)";
constexpr wchar_t kBethesdaRegPath[] = LR"(Software\Wow6432Node\Bethesda Softworks\)";

static constexpr Title kTitleMap[] = {{0, L"Unknown", "Unknown", L"Unknown"},
                                      {489830, L"Skyrim Special Edition", "SkyrimSE", L"SkyrimTogether.dll"},
                                      {611670, L"Skyrim VR", "SkyrimVR", L"SkyrimTogetherVR.dll"},
                                      {377160, L"Fallout 4", "Fallout4", L"FalloutTogether.dll"},
                                      {611660, L"Fallout 4 VR", "Fallout4VR", L"FalloutTogetherVR.dll"},
                                      {0, L"???", "???", L"???.dll"},
                                      {480, L"???", "Test", L"Test.dll"}};

// keep this in sync with enum!
static_assert(sizeof(kTitleMap) / sizeof(Title) == static_cast<size_t>(Title::Id::kCount),
              "Title map <=> Enum mismatch");

const Title* Title::ToTitle(Title::Id aTid) noexcept
{
    return &kTitleMap[static_cast<int>(aTid)];
}

// map command line alias to title id, by index
const Title* Title::ToTitle(const char* acAlias) noexcept
{
    for (int i = 0; i < static_cast<int>(Id::kCount); i++)
    {
        if (std::strcmp(kTitleMap[i].shortGameName, acAlias) == 0)
        {
            return &kTitleMap[i];
        }
    }

    return nullptr;
}

const Title::Id Title::ToTitleId(const Title& acTitle) noexcept
{
    for (int i = 0; i < static_cast<int>(Id::kCount); i++)
    {
        if (std::memcmp(&kTitleMap[i], &acTitle, sizeof(Title)) == 0)
        {
            return static_cast<Id>(i);
        }
    }

    return Id::kUnknown;
}

static std::wstring SuggestTitlePath(const Title& aTitle)
{
    WString path(kBethesdaRegPath);
    path += aTitle.fullGameName;

    const wchar_t* subName;
    switch (Title::ToTitleId(aTitle))
    {
    case Title::Id::kSkyrimSE:
    case Title::Id::kSkyrimVR:
        subName = L"installed path";
        break;
    case Title::Id::kFallout4:
    case Title::Id::kFallout4VR:
        subName = L"Installed Path";
        break;
    case Title::Id::kReserved1:
    case Title::Id::kReserved2:
    default:
        subName = L"";
    }

    return Registry::ReadString<wchar_t>(HKEY_LOCAL_MACHINE, path.c_str(), subName);
}

bool FindTitlePath(const Title& aTitle, bool aForceReselect, fs::path& aTitlePath, fs::path& aExePath)
{
    const WString regPath = WString(kTpRegistryPath) + aTitle.fullGameName;

    // separate, so a custom executable can be chosen for TP
    aTitlePath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, regPath.c_str(), L"TitlePath");
    aExePath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, regPath.c_str(), L"TitleExe");

    if (!fs::exists(aTitlePath) || !fs::exists(aExePath) || aForceReselect)
    {
        OPENFILENAMEW file{};

        std::wstring buffer;
        buffer.resize(MAX_PATH);

        file.lpstrFile = buffer.data();
        file.lStructSize = sizeof(file);
        file.nMaxFile = MAX_PATH;
        file.lpstrFilter = L"Executables\0*.exe\0";
        file.lpstrDefExt = L"EXE";
        file.lpstrTitle = L"Please select your Game executable (*.exe)";
        file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;

        auto initialDir = SuggestTitlePath(aTitle);
        file.lpstrInitialDir = initialDir.empty() ? buffer.data() : initialDir.data();

        if (!GetOpenFileNameW(&file))
        {
            FatalError(L"Failed to select path. Cannot launch!\nTry reselecting the path by launching TiltedOnline "
                       L"with the '-r' parameter.");
            return false;
        }

        size_t pos = buffer.find_last_of(L'\\');
        if (pos == std::string::npos)
            return false;

        aTitlePath = buffer.substr(0, pos);
        aExePath = buffer;

        return Registry::WriteString(HKEY_CURRENT_USER, regPath.c_str(), L"TitlePath", aTitlePath.native()) &&
               Registry::WriteString(HKEY_CURRENT_USER, regPath.c_str(), L"TitleExe", buffer);
    }

    return true;
}
