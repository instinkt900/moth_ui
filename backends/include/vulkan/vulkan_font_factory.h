#pragma once

#include "moth_ui/ifont_factory.h"
#include "moth_ui/ifont.h"
#include "vulkan_context.h"
#include "vulkan_graphics.h"
#include "vulkan_font_cache.h"

namespace backend::vulkan {
    class FontFactory : public moth_ui::IFontFactory {
    public:
        FontFactory(Context& context, Graphics& graphics);
        virtual ~FontFactory() = default;

        std::shared_ptr<moth_ui::IFont> GetDefaultFont(int size) override;
        std::vector<std::string> GetFontNameList() override;
        std::shared_ptr<moth_ui::IFont> GetFont(char const* name, int size) override;

    private:
        std::map<std::string, std::filesystem::path> m_fontPaths;
        FontCache m_fontCache;
    };
}
