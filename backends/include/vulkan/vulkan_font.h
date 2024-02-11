#pragma once

#include "moth_ui/ifont.h"
#include "vulkan_buffer.h"
#include "vulkan_graphics.h"

#include "harfbuzz/hb.h"

namespace backend::vulkan {
    class Font : public moth_ui::IFont {
    public:
        static std::shared_ptr<Font> Load(char const* path, int size, Context& context, Graphics& graphics);
        virtual ~Font();

        int32_t GetLineHeight() const { return m_lineHeight; }
        int32_t GetAscent() const { return m_ascent; }
        int32_t GetDescent() const { return m_descent; }
        int32_t GetUnderline() const { return m_underline; }

        moth_ui::IntVec2 const& GetGlyphBearing(int glyphIndex) const;

        int32_t GetStringWidth(std::string_view const& str) const;
        int32_t GetColumnHeight(std::string const& str, int32_t width) const;

        struct ShapedInfo {
            int glyphIndex;
            moth_ui::IntVec2 advance;
            moth_ui::IntVec2 offset;
        };
        std::vector<ShapedInfo> ShapeString(std::string_view const& str) const;

        struct LineDesc {
            int32_t lineWidth;
            std::string_view text;
        };
        std::vector<LineDesc> WrapString(std::string const& str, int32_t width) const;

        VkDescriptorSet GetVKDescriptorSet() const { return m_vkDescriptorSet; }

    private:
        Font(FT_Face face, int size, Context& context, Graphics& graphics);

        int CodepointToIndex(int codepoint) const;

        // harfbuzz stuff. this should probably be wrapped
        hb_font_t* m_hbFont = nullptr;
        mutable hb_buffer_t* m_hbBuffer = nullptr;

        // texture containing all glyphs
        std::unique_ptr<Image> m_glyphAtlas;

        // global font measurements
        int32_t m_lineHeight;
        int32_t m_ascent;
        int32_t m_descent;
        int32_t m_underline;

        struct ShaderInfo {
            moth_ui::IntVec2 Size;
            moth_ui::IntVec2 Unused;
            moth_ui::FloatVec2 UV0;
            moth_ui::FloatVec2 UV1;
        };

        std::map<int, uint32_t> m_codepointToAtlasIndex;    //
        std::vector<moth_ui::IntVec2> m_glyphBearings;      // glyph bearing values
        std::vector<ShaderInfo> m_shaderInfos;              // glyph info specifically for the shader

        VkDescriptorSet m_vkDescriptorSet;
        std::unique_ptr<Buffer> m_glyphInfosBuffer; // the buffer that stores the glyph infos
    };
}
