#pragma once

#include <include/internal/cef_ptr.h>

namespace TiltedPhoques
{
    struct OverlayApp;
}

struct RenderSystemD3D11;
struct D3D11RenderProvider;
struct FormIdComponent;
struct World;
struct ConnectedEvent;
struct DisconnectedEvent;
struct CellChangeEvent;
struct TransportService;
struct NotifyChatMessageBroadcast;
struct NotifyPlayerList;

using TiltedPhoques::OverlayApp;

struct OverlayService
{
    OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher);
    ~OverlayService() noexcept;

    TP_NOCOPYMOVE(OverlayService);

    void Create(RenderSystemD3D11* apRenderSystem) noexcept;

    void Render() const noexcept;
    void Reset() const noexcept;

    void Initialize() noexcept;

    void SetActive(bool aActive) noexcept;
    [[nodiscard]] bool GetActive() const noexcept;

    void SetInGame(bool aInGame) noexcept;
    [[nodiscard]] bool GetInGame() const noexcept;

    void SetVersion(const std::string& acVersion);

    OverlayApp* GetOverlayApp() const noexcept { return m_pOverlay.get(); }

    void SendSystemMessage(const std::string& acMessage);

  protected:
    void OnConnectedEvent(const ConnectedEvent&) noexcept;
    void OnDisconnectedEvent(const DisconnectedEvent&) noexcept;
    //void OnPlayerLeave(const PlayerLeaveEvent&) noexcept;
    void OnCellChangeEvent(const CellChangeEvent&) noexcept;
    void OnChatMessageReceived(const NotifyChatMessageBroadcast&) noexcept;
    void OnPlayerList(const NotifyPlayerList&) noexcept;

  private:
    CefRefPtr<OverlayApp> m_pOverlay{nullptr};
    UniquePtr<D3D11RenderProvider> m_pProvider;

    World& m_world;
    TransportService& m_transport;

    bool m_active = false;
    bool m_inGame = false;
    bool m_connected = false;

    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_cellChangeEventConnection;
    entt::scoped_connection m_chatMessageConnection;
    entt::scoped_connection m_playerListConnection;
};
