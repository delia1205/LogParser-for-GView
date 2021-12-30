#pragma once

#include "Internal.hpp"

namespace GView
{
namespace View
{
    namespace GridViewer
    {
        struct SettingsData
        {
            std::vector<std::vector<std::string>> content;
            char separator[2]{","};
            unsigned int rows = 0;
            unsigned int cols = 0;
            bool showHeader = false;
            SettingsData();
        };

        struct Config
        {
            struct
            {
                AppCUI::Input::Key ToggleHeader;
            } Keys;
            bool Loaded;

            static void Update(IniSection sect);
            void Initialize();
        };

        class Instance : public View::ViewControl
        {
          private:
            Reference<GView::Object> obj;
            FixSizeString<29> name;

            Reference<AppCUI::Controls::Grid> grid;
            Pointer<SettingsData> settings;

            static Config config;

          public:
            Instance(const std::string_view& name, Reference<GView::Object> obj, Settings* settings);

            bool GoTo(unsigned long long offset) override;
            bool Select(unsigned long long offset, unsigned long long size) override;
            std::string_view GetName() override;
            void PaintCursorInformation(AppCUI::Graphics::Renderer& renderer, unsigned int width, unsigned int height) override;

            virtual bool OnUpdateCommandBar(AppCUI::Application::CommandBar& commandBar) override;
            virtual bool OnEvent(Reference<Control>, Event eventType, int ID) override;

            // property interface
            bool GetPropertyValue(uint32 id, PropertyValue& value) override;
            bool SetPropertyValue(uint32 id, const PropertyValue& value, String& error) override;
            void SetCustomPropetyValue(uint32 propertyID) override;
            bool IsPropertyValueReadOnly(uint32 propertyID) override;
            const vector<Property> GetPropertiesList() override;

        private:
            void PopulateGrid();
        };
    } // namespace GridViewer
} // namespace View
}; // namespace GView