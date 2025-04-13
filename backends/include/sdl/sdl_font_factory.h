#pragma once

#include "moth_ui/font_factory.h"
#include "moth_ui/ifont.h"

#include <SDL.h>

namespace backend::sdl {
    class FontFactory : public moth_ui::FontFactory {
    public:
        FontFactory(SDL_Renderer& renderer);
        virtual ~FontFactory() = default;

        std::shared_ptr<moth_ui::IFont> GetFont(std::string const& name, int size) override;

    private:
        SDL_Renderer& m_renderer;
    };
}
