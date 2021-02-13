#pragma once

#include <include/internal/cef_ptr.h>

namespace TiltedPhoques
{
    struct OverlayApp;
}

struct RenderSystemD3D9;
struct RenderSystemD3D11;
struct FormIdComponent;
struct World;
struct ConnectedEvent;
struct DisconnectedEvent;
struct CellChangeEvent;
struct NotifyChatMessageBroadcast;
struct TransportService;

using TiltedPhoques::OverlayApp;

struct OverlayService
{
public:
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

    OverlayApp* GetOverlayApp() const noexcept { return m_pOverlay.get(); }

    void SendSystemMessage(const std::string& acMessage);


protected:

    void OnConnected(const ConnectedEvent&) noexcept;
    void OnDisconnectedEvent(const DisconnectedEvent&) noexcept;
    void OnCellChangeEvent(const CellChangeEvent&) noexcept;
    void OnChatMessageReceived(const NotifyChatMessageBroadcast&) noexcept;


private:

    CefRefPtr<OverlayApp> m_pOverlay{nullptr};
    World& m_world;
    TransportService& m_transport;

    bool m_active = false;
    bool m_inGame = false;
    bool m_connected = false;

    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_cellChangeEventConnection;
    entt::scoped_connection m_chatMessageConnection;
};
