#pragma once

#include "moth_ui/asset_id.h"
#include "moth_ui/graphics/iimage.h"

#include <memory>

namespace moth_ui {
    /**
     * @brief Abstract factory interface for loading and caching images.
     *
     * Consumers of the UI library must provide a concrete implementation that turns
     * an AssetId into an image, using the backend renderer. Where the bytes come from is
     * the implementation's business. moth_ui never reads the identity.
     */
    class IImageFactory {
    public:
        /**
         * @brief Returns the image that an identity names.
         *
         * moth_ui does not read the identity. The implementation decides what it means,
         * and a consumer that loads loose files can treat it as a path.
         *
         * @param id Names the image to load.
         * @return Newly created image, or @c nullptr on failure.
         */
        virtual std::unique_ptr<IImage> GetImage(AssetId const& id) = 0;

        IImageFactory() = default;
        IImageFactory(IImageFactory const&) = default;
        IImageFactory(IImageFactory&&) = default;
        IImageFactory& operator=(IImageFactory const&) = default;
        IImageFactory& operator=(IImageFactory&&) = default;
        virtual ~IImageFactory() = default;
    };
}
