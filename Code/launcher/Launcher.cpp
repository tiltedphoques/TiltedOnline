
#include <Debug.hpp>
#include <cxxopts.hpp>

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"
#include "SteamSupport.h"

#include "Utils/Error.h"
#include "loader/ExeLoader.h"
#include <BuildInfo.h>

static Launcher* g_pLauncher = nullptr;
constexpr uintptr_t kGameLoadLimit = 0x140000000 + 0x70000000;

Launcher* GetLauncher()
{
    return g_pLauncher;
}

Launcher::Launcher(int argc, char** argv)
{
    using TiltedPhoques::Debug;
    // only creates a new console if we aren't started from one
    Debug::CreateConsole();
    ParseCommandline(argc, argv);

    g_pLauncher = this;
}

Launcher::~Launcher()
{
    // explicit
    if (m_pClientHandle)
        FreeLibrary(m_pClientHandle);

    g_pLauncher = nullptr;
}

void Launcher::ParseCommandline(int argc, char** argv) noexcept
{
    cxxopts::Options options(argv[0], R"(Welcome to the TiltedOnline command line \(^_^)/)");

    std::string gameAlias = "";
    options.add_options()("h,help", "Display the help message")("v,version", "Display the build version")(
        "g,game", "game name (SkyrimSE or Fallout4)", cxxopts::value<std::string>(gameAlias))("r,reselect",
                                                                                              "Reselect the game path");

    try
    {
        const auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            fmt::print(options.help({""}));
            m_appState = AppState::kFailed;
            return;
        }

        if (result.count("version"))
            fmt::print("TiltedOnline version: " BUILD_BRANCH "@" BUILD_COMMIT "\n");

        if (!gameAlias.empty())
        {
            if (m_pTitle = Title::ToTitle(gameAlias.c_str()))
            {
                // signal that we don't want the ui
                m_appState = AppState::kInGame;
            }
            else
            {
                fmt::print("Unable to determine game type\n");
                m_appState = AppState::kFailed;
                return;
            }
        }

        m_bReselectFlag = result.count("reselect");
    }
    catch (const cxxopts::OptionException& ex)
    {
        m_appState = AppState::kFailed;
        fmt::print("Exception while parsing options: {}\n", ex.what());
    }
}

const fs::path& Launcher::GetGamePath() const
{
    return m_gamePath;
}

const fs::path& Launcher::GetExePath() const
{
    return m_exePath;
}

bool Launcher::Initialize()
{
    // there has been an error during startup
    if (m_appState == AppState::kFailed)
    {
        return false;
    }

    // no further initialization needed
    if (m_appState == AppState::kInGame)
    {
        return true;
    }

    // TBD: shared window + context init here
    return true;
}

void Launcher::InitPathEnvironment() noexcept
{
    auto appPath = TiltedPhoques::GetPath();
    SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
    AddDllDirectory(appPath.c_str());
    AddDllDirectory(m_gamePath.c_str());
    SetCurrentDirectoryW(m_gamePath.c_str());

    std::wstring pathBuf;
    pathBuf.resize(32768);
    GetEnvironmentVariableW(L"PATH", pathBuf.data(), static_cast<DWORD>(pathBuf.length()));

    // append bin & game directories
    std::wstring newPath = appPath.native() + L";" + m_gamePath.native() + L";" + pathBuf;
    SetEnvironmentVariableW(L"PATH", newPath.c_str());
}

void Launcher::StartGame()
{
    if (!m_pTitle)
    {
        FatalError(L"StartGame() -> Invalid title");
        return;
    }

    ExeLoader::TEntryPoint start = nullptr;
    {
        if (!FindTitlePath(*m_pTitle, m_bReselectFlag, m_gamePath, m_exePath))
            return;

        InitPathEnvironment();
        SteamLoad(*m_pTitle, m_gamePath);

        ExeLoader loader(kGameLoadLimit, GetProcAddress);
        if (!loader.Load(m_exePath))
            return;

        start = loader.GetEntryPoint();
    }

    Initializer::RunAll();
    start();
}

void Launcher::LoadClient() noexcept
{
    fs::path clientDll = TiltedPhoques::GetPath() / m_pTitle->tpClientName;
    fs::path cefDll = TiltedPhoques::GetPath() / L"libcef.dll";

    if (!(m_pClientHandle = LoadLibraryW(clientDll.c_str())))
    {
        if (!fs::exists(cefDll))
        {
            auto msg = fmt::format(L"Failed to load client\nLooks like CEF dlls are missing!\nPath: {}\nSuggested fix: "
                                   L"Run xmake install -o package",
                                   clientDll.native());
            FatalError(msg.c_str());
        }
        else
        {
            auto msg = fmt::format(L"Failed to load client\nPath: {}", clientDll.native());
            FatalError(msg.c_str());
        }

        TerminateProcess(GetCurrentProcess(), 0);
    }
}

int32_t Launcher::Exec() noexcept
{
    if (m_appState == AppState::kInGame)
    {
        StartGame();
        return 0;
    }

    // temporary selection code, until we have the new ui:
    fmt::print("Select game:\n"
               "1) SkyrimSE\n2) Fallout4\n");

    Title::Id tid{Title::Id::kUnknown};

    int c;
    while (c = std::getchar())
    {
        switch (c)
        {
        case '1':
            tid = Title::Id::kSkyrimSE;
            break;
        case '2':
            tid = Title::Id::kFallout4;
            break;
        case '\n':
            break;
        default:
            fmt::print("Invalid selection. Try again!\n");
            break;
        }

        if (tid != Title::Id::kUnknown)
            break;
    }

    m_pTitle = Title::ToTitle(tid);
    StartGame();
    return 0;
}
