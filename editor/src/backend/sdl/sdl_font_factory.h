#pragma once

#include "moth_ui/ifont_factory.h"
#include "moth_ui/ifont.h"

#include <SDL.h>

namespace backend::sdl {
    class FontFactory : public moth_ui::IFontFactory {
    public:
        FontFactory(SDL_Renderer& renderer);
        virtual ~FontFactory() = default;

        std::shared_ptr<moth_ui::IFont> GetDefaultFont(int size) override;
        std::vector<std::string> GetFontNameList() override;
        std::shared_ptr<moth_ui::IFont> GetFont(char const* name, int size) override;

    private:
        SDL_Renderer& m_renderer;
        std::map<std::string, std::filesystem::path> m_fontPaths;
    };
}
