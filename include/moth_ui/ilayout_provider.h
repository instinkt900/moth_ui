#pragma once

#include "moth_ui/asset_id.h"
#include "moth_ui/moth_ui_fwd.h"

#include <memory>

namespace moth_ui {
    /**
     * @brief Turns the identity of a sub-layout into a layout.
     *
     * A LayoutEntityRef names another layout. Without a provider it reads that file
     * off disk, relative to the directory the referencing layout came from. A
     * consumer that does not keep its layouts as loose files has no such directory,
     * so it supplies one of these instead and decides for itself where bytes come
     * from.
     *
     * moth_ui never reads the identity, the same way it never reads an image
     * identity. See @ref AssetId.
     *
     * @code
     * class MyProvider : public moth_ui::ILayoutProvider {
     *     std::shared_ptr<moth_ui::Layout> GetLayout(moth_ui::AssetId const& id) override {
     *         return m_database.ReadLayout(id.str());
     *     }
     * };
     *
     * MyProvider provider;
     * moth_ui::LayoutEntity::SerializeContext context;
     * context.m_layoutProvider = &provider;
     * @endcode
     */
    class ILayoutProvider {
    public:
        /**
         * @brief Returns the layout that an identity names.
         * @param id Names the layout to load.
         * @return The layout, or @c nullptr when it could not be read.
         */
        virtual std::shared_ptr<Layout> GetLayout(AssetId const& id) = 0;

        ILayoutProvider() = default;
        ILayoutProvider(ILayoutProvider const&) = default;
        ILayoutProvider(ILayoutProvider&&) = default;
        ILayoutProvider& operator=(ILayoutProvider const&) = default;
        ILayoutProvider& operator=(ILayoutProvider&&) = default;
        virtual ~ILayoutProvider() = default;
    };
}
