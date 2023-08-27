#include "common.h"
#include "vulkan/vulkan_font.h"
#include "stb_rect_pack.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "hb-ft.h"

#define _unused(x) ((void)(x))
#define FT_CHECK(r)         \
    {                       \
        FT_Error err = (r); \
        assert(!err);       \
        _unused(err);       \
    }                       \
    while (0)               \
        ;

namespace {
    int NextPowerOf2(int value) {
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;
        return value;
    }

    moth_ui::IntVec2 FindOptimalDimensions(std::vector<stbrp_node>& nodes, std::vector<stbrp_rect>& rects, moth_ui::IntVec2 const& minPack, moth_ui::IntVec2 const& maxPack) {
        // collect some info about all the rects
        int minWidth = std::numeric_limits<int>::max();
        int minHeight = std::numeric_limits<int>::max();
        int maxWidth = std::numeric_limits<int>::min();
        int maxHeight = std::numeric_limits<int>::min();
        int totalArea = 0;

        for (auto&& rect : rects) {
            minWidth = std::min(minWidth, rect.w);
            minHeight = std::min(minHeight, rect.h);
            maxWidth = std::max(maxWidth, rect.w);
            maxHeight = std::max(maxHeight, rect.h);
            totalArea += rect.w * rect.h;
        }

        struct PackTest {
            moth_ui::IntVec2 m_dimensions;
            float m_ratio;
        };

        int const minDimX = NextPowerOf2(minPack.x);
        int const minDimY = NextPowerOf2(minPack.y);
        int const maxDimX = NextPowerOf2(maxPack.x);
        int const maxDimY = NextPowerOf2(maxPack.y);

        std::vector<PackTest> testDimensions;
        int curWidth = minDimX;
        while (curWidth <= maxDimX) {
            int curHeight = minDimY;
            while (curHeight <= maxDimY) {
                int const curArea = curWidth * curHeight;
                if (curArea > totalArea) {
                    PackTest info;
                    info.m_dimensions = moth_ui::IntVec2{ curWidth, curHeight };
                    info.m_ratio = 0;
                    testDimensions.push_back(info);
                }
                curHeight *= 2;
            }
            curWidth *= 2;
        }

        for (auto&& testDim : testDimensions) {
            stbrp_context stbContext;
            stbrp_init_target(&stbContext, testDim.m_dimensions.x, testDim.m_dimensions.y, nodes.data(), static_cast<int>(nodes.size()));
            auto const allPacked = stbrp_pack_rects(&stbContext, rects.data(), static_cast<int>(rects.size()));
            if (allPacked) {
                float testArea = static_cast<float>(testDim.m_dimensions.x * testDim.m_dimensions.y);
                testDim.m_ratio = totalArea / testArea;
            }
        }

        std::sort(std::begin(testDimensions), std::end(testDimensions), [](auto const& a, auto const& b) { return b.m_ratio < a.m_ratio; });
        return std::begin(testDimensions)->m_dimensions;
    }
}

namespace backend::vulkan {
    std::shared_ptr<Font> Font::Load(char const* path, int size, Context& context, Graphics& graphics) {
        FT_Face face;
        if (FT_New_Face(context.m_ftLibrary, path, 0, &face) != 0) {
            return nullptr;
        }
        return std::shared_ptr<Font>(new Font(face, size, context, graphics));
    }

    Font::Font(FT_Face face, int size, Context& context, Graphics& graphics)
        : m_ftFace(face) {
        FT_CHECK(FT_Set_Pixel_Sizes(face, 0, size));

        m_hbFont = hb_ft_font_create(m_ftFace, nullptr);

        std::vector<stbrp_rect> stbRects;
        int minGlyphWidth, maxGlyphWidth;
        int minGlyphHeight, maxGlyphHeight;
        int fullWidth = 0;
        int fullHeight = 0;

        minGlyphWidth = minGlyphHeight = std::numeric_limits<int>::max();
        maxGlyphWidth = maxGlyphHeight = 0;

        std::vector<int> charcodes;

        // first we iterate through all the glyphs in the fontData. measuring them and preparing
        // the rects for the stb packer
        FT_ULong charcode;
        FT_UInt gindex;
        charcode = FT_Get_First_Char(face, &gindex);
        while (gindex != 0) {
            FT_CHECK(FT_Load_Glyph(face, gindex, FT_LOAD_DEFAULT));

            const int glyphWidth = face->glyph->bitmap.width;
            const int glyphHeight = face->glyph->bitmap.rows;

            minGlyphWidth = std::min(minGlyphWidth, glyphWidth);
            minGlyphHeight = std::min(minGlyphHeight, glyphHeight);
            maxGlyphWidth = std::max(maxGlyphWidth, glyphWidth);
            maxGlyphHeight = std::max(maxGlyphHeight, glyphHeight);
            fullWidth += glyphWidth;
            fullHeight += glyphHeight;

            stbrp_rect r;
            r.id = gindex;
            r.w = glyphWidth;
            r.h = glyphHeight;

            stbRects.push_back(r);

            m_glyphBearings.push_back({ face->glyph->metrics.horiBearingX / 64, face->glyph->metrics.horiBearingY / 64 });

            // want to store a list of charcodes in index order so we can
            // map charcode to rect.id/glyph index later
            charcodes.push_back(charcode);

            charcode = FT_Get_Next_Char(face, charcode, &gindex);
        }

        // now we use the stb packer to optimally pack them in an arrangement
        std::vector<stbrp_node> stbNodes(fullWidth * 2);
        auto const packDim = FindOptimalDimensions(stbNodes, stbRects, { 1, 1 }, { fullWidth, fullHeight });
        stbrp_context stbContext;
        stbrp_init_target(&stbContext, packDim.x, packDim.y, stbNodes.data(), static_cast<int>(stbNodes.size()));
        stbrp_pack_rects(&stbContext, stbRects.data(), static_cast<int>(stbRects.size()));

        const int dataStride = packDim.x * 4;
        std::vector<unsigned char> packData(dataStride * packDim.y);
        moth_ui::FloatVec2 const texSize(static_cast<float>(packDim.x), static_cast<float>(packDim.y));

        // now we render the glyph into some texture data and store the position
        // and size of the glyph for the charcode
        for (auto&& rect : stbRects) {
            // render glyph
            FT_GlyphSlot glyphSlot = face->glyph;
            FT_CHECK(FT_Load_Glyph(face, rect.id, FT_LOAD_RENDER));
            int const glyphStride = face->glyph->bitmap.pitch;
            int const glyphPosX = rect.x;
            int const glyphPosY = rect.y;
            int const glyphWidth = rect.w;
            int const glyphHeight = rect.h;
            for (int y = 0; y < glyphHeight; ++y) {
                for (int x = 0; x < glyphWidth; ++x) {
                    unsigned char alpha = face->glyph->bitmap.buffer[x + y * glyphStride];
                    packData[((glyphPosX + x) * 4) + ((glyphPosY + y) * dataStride) + 0] = alpha;
                    packData[((glyphPosX + x) * 4) + ((glyphPosY + y) * dataStride) + 1] = alpha;
                    packData[((glyphPosX + x) * 4) + ((glyphPosY + y) * dataStride) + 2] = alpha;
                    packData[((glyphPosX + x) * 4) + ((glyphPosY + y) * dataStride) + 3] = alpha;
                }
            }

            // store info
            int const glyphIndex = static_cast<int>(m_shaderInfos.size());
            int const glyphCode = charcodes[glyphIndex];
            moth_ui::FloatVec2 const pos0(static_cast<float>(glyphPosX), static_cast<float>(glyphPosY));
            moth_ui::FloatVec2 const pos1(static_cast<float>(glyphPosX + glyphWidth), static_cast<float>(glyphPosY + glyphHeight));
            moth_ui::FloatVec2 const uv0 = pos0 / texSize;
            moth_ui::FloatVec2 const uv1 = pos1 / texSize;
            // https://stackoverflow.com/questions/66265216/how-is-freetype-calculating-advance
            m_shaderInfos.push_back({ { glyphWidth, glyphHeight }, { glyphSlot->advance.x / 64, glyphSlot->advance.y / 64 }, uv0, uv1 });
            m_charCodeToIndex.insert(std::make_pair(glyphCode, glyphIndex));
            m_codepointToIndex.insert(std::make_pair(rect.id, glyphIndex));
        }

        m_glyphAtlas = Image::FromRGBA(context, packDim.x, packDim.y, packData.data());
        m_lineHeight = face->size->metrics.height / 64;
        m_ascent = face->size->metrics.ascender / 64;

        int const dataSize = static_cast<int>(m_shaderInfos.size() * sizeof(ShaderInfo));
        m_glyphInfosBuffer = std::make_unique<Buffer>(context, dataSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        std::unique_ptr<Buffer> staging = std::make_unique<Buffer>(context, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        unsigned char* stagingPtr = static_cast<unsigned char*>(staging->Map());
        memcpy(stagingPtr, m_shaderInfos.data(), dataSize);
        staging->Unmap();
        m_glyphInfosBuffer->Copy(*staging);

        Shader& fontShader = graphics.GetFontShader();

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = fontShader.m_descriptorPool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &fontShader.m_descriptorSetLayout;
        CHECK_VK_RESULT(vkAllocateDescriptorSets(context.m_vkDevice, &alloc_info, &m_vkDescriptorSet));

        VkDescriptorBufferInfo glyph_info[1] = {};
        glyph_info[0].buffer = m_glyphInfosBuffer->GetVKBuffer();
        glyph_info[0].offset = 0;
        glyph_info[0].range = m_shaderInfos.size() * sizeof(ShaderInfo);

        VkDescriptorImageInfo desc_image[1] = {};
        desc_image[0].sampler = m_glyphAtlas->GetVkSampler();
        desc_image[0].imageView = m_glyphAtlas->GetVkView();
        desc_image[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet write_desc[3] = {};

        write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc[0].dstSet = m_vkDescriptorSet;
        write_desc[0].dstBinding = 0;
        write_desc[0].dstArrayElement = 0;
        write_desc[0].descriptorCount = 1;
        write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write_desc[0].pBufferInfo = glyph_info;

        write_desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc[1].dstSet = m_vkDescriptorSet;
        write_desc[1].dstBinding = 1;
        write_desc[1].descriptorCount = 1;
        write_desc[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_desc[1].pImageInfo = desc_image;

        vkUpdateDescriptorSets(context.m_vkDevice, 2, write_desc, 0, nullptr);
    }

    Font::~Font() {
        hb_font_destroy(m_hbFont);
        hb_buffer_destroy(m_hbBuffer);
    }

    int Font::GetGlyphIndex(int charCode) const {
        auto const it = m_charCodeToIndex.find(charCode);
        if (std::end(m_charCodeToIndex) == it) {
            return -1;
        }
        return it->second;
    }

    moth_ui::IntVec2 const& Font::GetGlyphSize(int glyphIndex) const {
        return m_shaderInfos.at(glyphIndex).Size;
    }

    moth_ui::IntVec2 const& Font::GetGlyphBearing(int glyphIndex) const {
        return m_glyphBearings.at(glyphIndex);
    }

    int32_t Font::GetStringWidth(std::string_view const& str) const {
        int32_t width = 0;

        auto const shapedInfo = ShapeString(str);

        for (auto const& info : shapedInfo) {
            width += info.advance.x;
        }

        return width;
    }

    int32_t Font::GetColumnHeight(std::string const& str, int32_t width) const {
        auto const lines = WrapString(str, width);
        return static_cast<int32_t>(m_lineHeight * lines.size());
    }

    std::vector<Font::ShapedInfo> Font::ShapeString(std::string_view const& str) const {
        if (m_hbBuffer == nullptr) {
            m_hbBuffer = hb_buffer_create();
        } else {
            hb_buffer_clear_contents(m_hbBuffer);
        }

        hb_buffer_set_direction(m_hbBuffer, HB_DIRECTION_LTR);
        hb_buffer_set_script(m_hbBuffer, HB_SCRIPT_LATIN);
        hb_buffer_set_language(m_hbBuffer, hb_language_from_string("en", -1));
        hb_buffer_guess_segment_properties(m_hbBuffer);

        hb_buffer_add_utf8(m_hbBuffer, str.data(), static_cast<int>(str.length()), 0, -1);
        hb_shape(m_hbFont, m_hbBuffer, nullptr, 0);

        uint32_t glyphCount;
        hb_glyph_info_t* outGlyphInfo = hb_buffer_get_glyph_infos(m_hbBuffer, &glyphCount);
        hb_glyph_position_t* outGlyphPos = hb_buffer_get_glyph_positions(m_hbBuffer, &glyphCount);

        std::vector<Font::ShapedInfo> result;

        for (uint32_t i = 0; i < glyphCount; ++i) {
            result.push_back({ m_codepointToIndex.at(outGlyphInfo[i].codepoint),
                               { outGlyphPos[i].x_advance / 64, outGlyphPos[i].y_advance / 64 },
                               { outGlyphPos[i].x_offset / 64, outGlyphPos[i].y_offset / 64 } });
        }

        return result;
    }

    std::vector<Font::LineDesc> Font::WrapString(std::string const& str, int32_t width) const {
        std::vector<Font::LineDesc> lines;

        auto const SubmitNewLine = [this, &lines](char const* lineStart, size_t lineLength) {
            // strip preceeding whitespace
            while (lineLength > 0) {
                if (!std::isspace(*lineStart)) {
                    break;
                }
                ++lineStart;
                --lineLength;
            }

            // strip trailing whitespace
            while (lineLength > 0) {
                if (!std::isspace(lineStart[lineLength - 1])) {
                    break;
                }
                --lineLength;
            }

            // if theres anything left, add it to the list
            if (lineLength > 0) {
                std::string_view const view{ lineStart, lineLength };
                int32_t const lineWidth = GetStringWidth(view);
                lines.emplace_back(LineDesc{ lineWidth, view });
            }
        };

        // first break up using newlines already in the string
        std::vector<std::string_view> candidateLines;
        char const* lineStart = str.c_str();
        char const* ptr = lineStart;
        for (/* empty */; *ptr != 0; ++ptr) {
            size_t const curLineLength = ptr - lineStart;
            if (*ptr == '\n' && curLineLength > 0) {
                candidateLines.push_back({ lineStart, curLineLength });
                lineStart = ptr + 1;
            }
        }
        size_t const lastLineLength = ptr - lineStart;
        if (lastLineLength > 0) {
            candidateLines.push_back({ lineStart, lastLineLength });
        }

        // now wrap lines to the given width
        for (auto& candidateLine : candidateLines) {
            size_t beginIdx = 0;
            size_t lastBreakIdx = 0;
            size_t i = 0;
            for (/* empty */; i < candidateLine.length(); ++i) {
                if (std::isspace(candidateLine[i])) {
                    int32_t const thisLineWidth = GetStringWidth({ candidateLine.data() + beginIdx, i - beginIdx });
                    if (thisLineWidth > width) {
                        // adding this word puts us over the limit
                        if ((lastBreakIdx - beginIdx) > 0) {
                            // the last break position was not the start of the line
                            SubmitNewLine(candidateLine.data() + beginIdx, lastBreakIdx - beginIdx);
                            beginIdx = lastBreakIdx + 1;
                            i = beginIdx - 1; // to account for the inc at the end of the loop
                        } else {
                            // the last break position was the line start itself (the word is longer than width)
                            SubmitNewLine(candidateLine.data() + beginIdx, i - beginIdx);
                            beginIdx = i + 1;
                        }
                        lastBreakIdx = beginIdx;
                    } else {
                        // if we didnt go over width, just remember the position and width here
                        lastBreakIdx = i;
                    }
                }
            }

            // check if we have left over text
            if ((i - beginIdx) > 0) {
                std::string_view const lastView{ candidateLine.data() + beginIdx, i - beginIdx };
                int32_t const thisLineWidth = GetStringWidth(lastView);
                if (thisLineWidth > width) {
                    // adding this word puts us over the limit
                    if ((lastBreakIdx - beginIdx) > 0) {
                        // the last break position was not the start of the line
                        SubmitNewLine(candidateLine.data() + beginIdx, lastBreakIdx - beginIdx);
                        SubmitNewLine(candidateLine.data() + lastBreakIdx + 1, i - (lastBreakIdx + 1));
                    } else {
                        // the last break position was the line start itself (the word is longer than width)
                        SubmitNewLine(candidateLine.data() + beginIdx, i - beginIdx);
                    }
                } else {
                    SubmitNewLine(lastView.data(), lastView.length());
                }
            }
        }

        return lines;
    }
}
