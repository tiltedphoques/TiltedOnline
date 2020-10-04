#include <Services/OverlayClient.h>
#include <OverlayRenderHandler.hpp>

#include <Services/TransportService.h>

OverlayClient::OverlayClient(TransportService& aTransport, TiltedPhoques::OverlayRenderHandler* apHandler)
    : TiltedPhoques::OverlayClient(apHandler), m_transport(aTransport)
{
}

OverlayClient::~OverlayClient() noexcept
{
}

bool OverlayClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                             CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    if (message->GetName() == "ui-event")
    {
        auto pArguments = message->GetArgumentList();

        auto eventName = pArguments->GetString(0).ToString();
        auto eventArgs = pArguments->GetList(1);

#ifndef PUBLIC_BUILD
        LOG(INFO) << "event=ui_event name=" << eventName;
#endif

        if (eventName == "connect")
        {
            std::string baseIp = eventArgs->GetString(0);
            uint16_t port = eventArgs->GetInt(1) ? eventArgs->GetInt(1) : 10578;
            m_transport.Connect(baseIp + ":" + std::to_string(port));
        }

        return true;
    }

    return false;
}
