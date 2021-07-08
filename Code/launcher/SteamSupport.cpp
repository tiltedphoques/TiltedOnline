
#include <cstdio>

#include "SteamSupport.h"
#include "Utils/Registry.h"
#include <MinHook.h>
#include <TiltedCore/Filesystem.hpp>
#include <intrin.h>

namespace fs = std::filesystem;

const wchar_t kSteamDllName[] = L"steamclient64.dll";

void SteamLoad(const Title& aTitle, const fs::path& aGamePath)
{
    auto appId = std::to_wstring(aTitle.steamAppId);
    SetEnvironmentVariableW(L"SteamAppId", appId.c_str());

    auto path = aGamePath / "steam_appid.txt";

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, path.c_str(), L"w");
    if (pFile)
    {
        fwrite(appId.c_str(), appId.length(), 1, pFile);
        fclose(pFile);
    }

    fs::path steamPath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, LR"(SOFTWARE\Valve\Steam)", L"SteamPath");

    // wait what
    if (!fs::exists(steamPath))
        return;

    fs::path clientPath = steamPath / kSteamDllName;
    AddDllDirectory(steamPath.c_str());
    LoadLibraryW(clientPath.c_str());
}
