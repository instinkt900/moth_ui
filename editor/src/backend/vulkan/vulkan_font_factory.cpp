#include "common.h"
#include "vulkan_font_factory.h"
#include "vulkan_font.h"

namespace backend::vulkan {
    FontFactory::FontFactory(Context& context, Graphics& graphics)
        : m_context(context)
        , m_graphics(graphics) {
        m_fontPaths["Pilot Command"] = std::filesystem::current_path() / "pilotcommand.ttf";
        m_fontPaths["Daniel Davis"] = std::filesystem::current_path() / "Daniel Davis.ttf";
        m_fontPaths["Game of Squids"] = std::filesystem::current_path() / "Game Of Squids.ttf";
        m_fontPaths["Southern Aire"] = std::filesystem::current_path() / "SouthernAire_Personal_Use_Only.ttf";
        m_fontPaths["28 Days Later"] = std::filesystem::current_path() / "28 Days Later.ttf";
    }

    std::unique_ptr<moth_ui::IFont> FontFactory::GetDefaultFont(int size) {
        return GetFont(m_fontPaths.begin()->first.c_str(), size);
    }

    std::vector<std::string> FontFactory::GetFontNameList() {
        std::vector<std::string> nameList;
        for (auto& [fontName, fontPath] : m_fontPaths) {
            nameList.push_back(fontName);
        }
        return nameList;
    }

    std::unique_ptr<moth_ui::IFont> FontFactory::GetFont(char const* name, int size) {
        assert(!m_fontPaths.empty() && "No known fonts.");
        auto const it = m_fontPaths.find(name);
        if (std::end(m_fontPaths) == it) {
            return GetDefaultFont(size);
        }
        return Font::Load(it->second.string().c_str(), size, m_context, m_graphics);
    }
}
