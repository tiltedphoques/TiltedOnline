#pragma once

#include <spdlog/spdlog.h>

#include "TitleSelect.h"
#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Platform.hpp>
#include <TiltedCore/Stl.hpp>

class Launcher
{
  public:
    explicit Launcher(int argc, char** argv);
    ~Launcher();

    bool Initialize();
    void LoadClient() noexcept;
    void StartGame();

    int32_t Exec() noexcept;

    const fs::path& GetGamePath() const;
    const fs::path& GetExePath() const;

  private:
    void InitPathEnvironment() noexcept;
    void ParseCommandline(int argc, char** argv) noexcept;

    enum class AppState
    {
        kFailed,
        kStarting,
        kInGame,
        kBackground
    } m_appState{AppState::kStarting};
    const Title* m_pTitle = nullptr;

    bool m_bReselectFlag = false;
    HMODULE m_pClientHandle = nullptr;
    fs::path m_gamePath;
    fs::path m_exePath;
};

Launcher* GetLauncher();
