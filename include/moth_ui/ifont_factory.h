#pragma once

#include "moth_ui/ifont.h"

#include <filesystem>
#include <vector>
#include <memory>

namespace moth_ui {
    class IFontFactory {
    public:
        virtual ~IFontFactory() = default;

        virtual void AddFont(char const* name, std::filesystem::path const& path) = 0;
        virtual void RemoveFont(char const* name) = 0;
        virtual void LoadProject(std::filesystem::path const& path) = 0;
        virtual void SaveProject(std::filesystem::path const& path) = 0;
        virtual std::filesystem::path GetCurrentProjectPath() const = 0;
        virtual void ClearFonts() = 0;
        virtual std::shared_ptr<IFont> GetDefaultFont(int size) = 0;
        virtual std::vector<std::string> GetFontNameList() const = 0;
        virtual std::shared_ptr<IFont> GetFont(std::string const& name, int size) = 0;
        virtual std::filesystem::path GetFontPath(std::string const& name) const = 0;
    };
}

