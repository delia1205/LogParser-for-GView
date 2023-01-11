#include "PCAP.hpp"

#include <array>

using namespace AppCUI;
using namespace AppCUI::OS;
using namespace AppCUI::Utils;
using namespace AppCUI::Application;
using namespace AppCUI::Controls;
using namespace GView::Utils;
using namespace GView::Type;
using namespace GView;
using namespace GView::View;

extern "C"
{
    PLUGIN_EXPORT bool Validate(const AppCUI::Utils::BufferView& buf, const std::string_view& extension)
    {
        CHECK(buf.GetLength() > sizeof(PCAP::Header), false, "");

        auto header = buf.GetObject<PCAP::Header>(0);
        CHECK(header.IsValid(), false, "");

        CHECK(header->magicNumber == PCAP::Magic::Identical || header->magicNumber == PCAP::Magic::Swapped, false, "");

        return true;
    }

    PLUGIN_EXPORT TypeInterface* CreateInstance()
    {
        return new PCAP::PCAPFile();
    }

    static constexpr auto DarkGreenBlue = ColorPair{ Color::DarkGreen, Color::DarkBlue };
    static constexpr auto DarkRedBlue   = ColorPair{ Color::DarkRed, Color::DarkBlue };
    constexpr static auto colors        = std::array<ColorPair, 2>{ DarkGreenBlue, DarkRedBlue };

    void CreateBufferView(Reference<GView::View::WindowInterface> win, Reference<PCAP::PCAPFile> pcap)
    {
        BufferViewer::Settings settings;

        auto offset = 0ULL;
        settings.AddZone(offset, sizeof(pcap->header), ColorPair{ Color::Magenta, Color::DarkBlue }, "Header");
        offset += sizeof(pcap->header);

        auto count = 0;
        LocalString<32> ls;
        for (const auto& [header, offset] : pcap->packetHeaders)
        {
            const auto& c = *(colors.begin() + (count % 2));
            settings.AddZone(offset, sizeof(PCAP::PacketHeader) + header->inclLen, c, ls.Format("Packet_%u", count));
            count++;
        }

        win->CreateViewer("BufferView", settings);
    }

    PLUGIN_EXPORT bool PopulateWindow(Reference<GView::View::WindowInterface> win)
    {
        auto pcap = win->GetObject()->GetContentType<PCAP::PCAPFile>();
        pcap->Update();

        // add views
        CreateBufferView(win, pcap);

        // add panels
        win->AddPanel(Pointer<TabPage>(new PCAP::Panels::Information(win->GetObject(), pcap)), true);
        win->AddPanel(Pointer<TabPage>(new PCAP::Panels::Packets(pcap, win)), false);

        return true;
    }

    PLUGIN_EXPORT void UpdateSettings(IniSection sect)
    {
        sect["Pattern"]     = { "magic:A1 B2 C3 D4", "magic:D4 C3 B2 A1" };
        sect["Extension"]   = "pcap";
        sect["Priority"]    = 1;
        sect["Description"] = "Network Packet capture file format";
    }
}
