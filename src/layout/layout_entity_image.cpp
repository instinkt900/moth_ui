#include "common.h"
#include "moth_ui/graphics/texture_filter.h"
#include "moth_ui/layout/layout_entity_image.h"
#include "moth_ui/nodes/node_image.h"

namespace moth_ui {
    LayoutEntityImage::LayoutEntityImage(LayoutRect const& initialBounds)
        : LayoutEntity(initialBounds) {
    }

    LayoutEntityImage::LayoutEntityImage(LayoutEntityGroup* parent)
        : LayoutEntity(parent) {
    }

    LayoutEntityImage::LayoutEntityImage(LayoutRect const& initialBounds, AssetId imageId)
        : LayoutEntity(initialBounds)
        , m_imageId(std::move(imageId)) {
    }

    std::shared_ptr<LayoutEntity> LayoutEntityImage::Clone(CloneType cloneType) {
        return std::make_shared<LayoutEntityImage>(*this);
    }

    std::shared_ptr<Node> LayoutEntityImage::Instantiate(Context& context) {
        return NodeImage::Create(context, std::static_pointer_cast<LayoutEntityImage>(shared_from_this()));
    }

    nlohmann::json LayoutEntityImage::Serialize(SerializeContext const& context) const {
        nlohmann::json j = LayoutEntity::Serialize(context);

        // The identity is written exactly as it was given. moth_ui used to make this
        // path relative here and absolute again on the way back in, which meant a
        // consumer could not store anything that was not a path. See AssetId.
        j["imagePath"] = m_imageId.str();
        j["sourceRect"] = m_sourceRect;
        j["imageScaleType"] = m_imageScaleType;
        j["imageScale"] = m_imageScale;
        j["textureFilter"] = m_textureFilter;
        j["sourceBorders"] = m_sourceBorders;
        j["targetBorders"] = m_targetBorders;
        return j;
    }

    bool LayoutEntityImage::Deserialize(nlohmann::json const& json, SerializeContext const& context) {
        bool success = LayoutEntity::Deserialize(json, context);

        if (success) {
            m_sourceRect = json.value("sourceRect", IntRect{});
            m_imageScaleType = json.value("imageScaleType", ImageScaleType::Stretch);
            m_imageScale = json.value("imageScale", 1.0f);
            auto const rawFilter = json.value("textureFilter", TextureFilter::Linear);
            m_textureFilter = (rawFilter == TextureFilter::Invalid) ? TextureFilter::Linear : rawFilter;
            m_sourceBorders = json.value("sourceBorders", IntRect{});
            m_targetBorders = json.value("targetBorders", MakeDefaultLayoutRect());
            m_imageId = AssetId{ json.value("imagePath", std::string{}) };
        }

        return success;
    }
}
