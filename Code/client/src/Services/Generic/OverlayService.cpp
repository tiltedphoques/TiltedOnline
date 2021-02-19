#include <Services/OverlayService.h>

#include <OverlayApp.hpp>

#include <D3D11Hook.hpp>

#include <OverlayRenderHandlerD3D11.hpp>

#include <Systems/RenderSystemD3D11.h>

#include <Components.h>
#include <World.h>

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/CellChangeEvent.h>

#include <Services/OverlayClient.h>
#include <Services/TransportService.h>

using TiltedPhoques::OverlayRenderHandlerD3D11;
using TiltedPhoques::OverlayRenderHandler;


struct D3D11RenderProvider final : OverlayApp::RenderProvider, OverlayRenderHandlerD3D11::Renderer
{
    explicit D3D11RenderProvider(RenderSystemD3D11* apRenderSystem) : m_pRenderSystem(apRenderSystem) {}

    OverlayRenderHandler* Create() override
    {
        return new OverlayRenderHandlerD3D11(this);
    }

    [[nodiscard]] HWND GetWindow() override
    {
        return m_pRenderSystem->GetWindow();
    }

    [[nodiscard]] IDXGISwapChain* GetSwapChain() const noexcept override
    {
        return m_pRenderSystem->GetSwapChain();
    }

private:

    RenderSystemD3D11* m_pRenderSystem;
};

OverlayService::OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(transport)
{
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&OverlayService::OnConnected>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&OverlayService::OnDisconnectedEvent>(this);
    m_cellChangeEventConnection = aDispatcher.sink<CellChangeEvent>().connect<&OverlayService::OnCellChangeEvent>(this);
}

OverlayService::~OverlayService() noexcept
{
}

void OverlayService::Create(RenderSystemD3D11* apRenderSystem) noexcept
{
    m_pOverlay = new OverlayApp(std::make_unique<D3D11RenderProvider>(apRenderSystem));

    m_world.set<OverlayClient>(m_transport, m_pOverlay->GetRenderProvider()->Create());
    OverlayClient& overlayClient = m_world.ctx<OverlayClient>();
    m_pOverlay->Initialize(&overlayClient);
    m_pOverlay->GetClient()->Create();
}

void OverlayService::Render() const noexcept
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;
    
    m_pOverlay->GetClient()->Render();
}

void OverlayService::Reset() const noexcept
{
    m_pOverlay->GetClient()->Reset();
}

void OverlayService::Initialize() noexcept
{
}

void OverlayService::SetActive(bool aActive) noexcept
{
    /*
        if (!m_inGame)
        {
            return;
        }
        */

    m_active = aActive;

    m_pOverlay->ExecuteAsync(m_active ? "activate" : "deactivate");
}

bool OverlayService::GetActive() const noexcept
{
    return m_active;
}

void OverlayService::SetInGame(bool aInGame) noexcept
{
    if (m_inGame == aInGame)
        return;
    m_inGame = aInGame;

    if (m_inGame)
    {
        m_pOverlay->ExecuteAsync("entergame");
    }
    else
    {
        m_pOverlay->ExecuteAsync("exitgame");
        SetActive(false);
    }
}

bool OverlayService::GetInGame() const noexcept
{
    return m_inGame;
}

void OverlayService::SendSystemMessage(const std::string& acMessage)
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetString(0, acMessage);
    m_pOverlay->ExecuteAsync("systemmessage", pArguments);
}

void OverlayService::OnConnected(const ConnectedEvent&) noexcept
{
    m_connected = true;
    m_pOverlay->ExecuteAsync("connect");
    SendSystemMessage("Successful connections to server");
}

void OverlayService::OnDisconnectedEvent(const DisconnectedEvent&) noexcept
{
    m_connected = false;
    m_pOverlay->ExecuteAsync("disconnect");
    SendSystemMessage("Disconnected from server");
}

void OverlayService::OnCellChangeEvent(const CellChangeEvent& aCellChangeEvent) noexcept
{
    spdlog::warn("OnCellChangeEvent ! %s", aCellChangeEvent.Name);
    // Hacky as fuck... Idk why the first cellchangeevent broke UI. It's not a big deal because when player connected we force cell changed event
    // But change event come before connected event.
    static bool firstCellChangeEvent = false;
    if (!m_pOverlay || !firstCellChangeEvent)
    {
        firstCellChangeEvent = true;
        return;
    }
    SendSystemMessage("On Cell change event");
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, 1);
    pArguments->SetString(1, "TestCell");
    m_pOverlay->ExecuteAsync("setcell", pArguments);
    spdlog::warn("OnCellChangeEvent end !");
}
