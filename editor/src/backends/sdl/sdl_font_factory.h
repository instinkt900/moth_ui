#pragma once

#include "moth_ui/ifont_factory.h"
#include "moth_ui/ifont.h"

class SDLFontFactory : public moth_ui::IFontFactory {
public:
    SDLFontFactory(SDL_Renderer& renderer);
    virtual ~SDLFontFactory() = default;

    std::unique_ptr<moth_ui::IFont> GetDefaultFont(int size) override;
    std::vector<std::string> GetFontNameList() override;
    std::unique_ptr<moth_ui::IFont> GetFont(char const* name, int size) override;

private:
    SDL_Renderer& m_renderer;
    std::map<std::string, std::filesystem::path> m_fontPaths;
};
