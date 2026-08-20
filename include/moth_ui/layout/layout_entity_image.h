#pragma once

#include "moth_ui/asset_id.h"
#include "moth_ui/graphics/image_scale_type.h"
#include "moth_ui/graphics/texture_filter.h"
#include "moth_ui/layout/layout_entity.h"
#include "moth_ui/layout/layout_rect.h"
#include "moth_ui/utils/rect.h"

#include <nlohmann/json_fwd.hpp>

namespace moth_ui {
    /**
     * @brief Layout entity that describes an image node.
     *
     * Stores the image identity, source rectangle, scale type, and nine-slice
     * border settings used when instantiating a NodeImage.
     */
    class LayoutEntityImage : public LayoutEntity {
    public:
        /**
         * @brief Constructs an image entity with an explicit initial bounds.
         * @param initialBounds Starting layout rect.
         */
        explicit LayoutEntityImage(LayoutRect const& initialBounds);

        /**
         * @brief Constructs an image entity as a child of the given parent.
         * @param parent Owning group.
         */
        explicit LayoutEntityImage(LayoutEntityGroup* parent);

        /**
         * @brief Constructs an image entity that names a specific image.
         * @param initialBounds Starting layout rect.
         * @param imageId       Names the image to load.
         */
        LayoutEntityImage(LayoutRect const& initialBounds, AssetId imageId);

        std::shared_ptr<LayoutEntity> Clone(CloneType cloneType) override;

        /// @brief Returns @c LayoutEntityType::Image.
        LayoutEntityType GetType() const override { return LayoutEntityType::Image; }

        std::shared_ptr<Node> Instantiate(Context& context) override;

        nlohmann::json Serialize(SerializeContext const& context) const override;
        bool Deserialize(nlohmann::json const& json, SerializeContext const& context) override;

        AssetId m_imageId;                                    ///< Names the image to load.
        IntRect m_sourceRect;                                 ///< Portion of the image to display.
        ImageScaleType m_imageScaleType = ImageScaleType::Stretch; ///< How the image is scaled.
        float m_imageScale = 1.0f;                            ///< Uniform scale factor for tile/nine-slice modes.
        TextureFilter m_textureFilter = TextureFilter::Linear; ///< Sampling filter applied when the image is scaled.

        // 9slice only
        static int constexpr kDefaultBorderSize = 15;                                               ///< Default nine-slice border width in pixels.
        IntRect m_sourceBorders = { { kDefaultBorderSize, kDefaultBorderSize }, { kDefaultBorderSize, kDefaultBorderSize } }; ///< Source-space border insets for nine-slice.
        LayoutRect m_targetBorders = MakeDefaultLayoutRect();                                      ///< Target-space border fractions for nine-slice.

        LayoutEntityImage(LayoutEntityImage const& other) = default;
        LayoutEntityImage(LayoutEntityImage&& other) = default;
        LayoutEntityImage& operator=(LayoutEntityImage const&) = delete;
        LayoutEntityImage& operator=(LayoutEntityImage&&) = default;
        ~LayoutEntityImage() override = default;
    };
}
