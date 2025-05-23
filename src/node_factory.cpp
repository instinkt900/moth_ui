#include "common.h"
#include "moth_ui/node_factory.h"
#include "moth_ui/layout/layout.h"
#include "moth_ui/nodes/node.h"
#include "moth_ui/nodes/group.h"

namespace moth_ui {
    std::unique_ptr<NodeFactory> NodeFactory::s_instance;

    std::string NodeFactory::RegisterWidget(std::string const& className, CreationFunction const& func) {
        m_creationFunctions[className] = func;
        return className;
    }

    std::unique_ptr<Group> NodeFactory::Create(Context& context, std::filesystem::path const& path, int width, int height) {
        std::shared_ptr<Layout> layout;
        auto const loadResult = Layout::Load(path, &layout);
        if (loadResult == Layout::LoadResult::Success) {
            std::unique_ptr<Group> resultNode = Create(context, std::static_pointer_cast<LayoutEntityGroup>(layout));

            IntRect initialRect;
            initialRect.topLeft = { 0, 0 };
            initialRect.bottomRight = { width, height };
            resultNode->SetScreenRect(initialRect);

            return resultNode;
        }
        return nullptr;
    }

    std::unique_ptr<Group> NodeFactory::Create(Context& context, std::shared_ptr<LayoutEntityGroup> group) {
        std::unique_ptr<Group> resultNode;

        if (!group->m_class.empty()) {
            auto const it = m_creationFunctions.find(group->m_class);
            if (std::end(m_creationFunctions) != it) {
                resultNode = it->second(context, group);
            }
        }

        if (!resultNode) {
            resultNode = std::unique_ptr<Group>(static_cast<Group*>(group->Instantiate(context).release()));
        }

        return resultNode;
    }

    std::unique_ptr<Node> NodeFactory::Create(Context& context, std::shared_ptr<LayoutEntity> entity) {
        switch (entity->GetType()) {
        case LayoutEntityType::Group:
        case LayoutEntityType::Ref:
        case LayoutEntityType::Layout:
            return Create(context, std::static_pointer_cast<LayoutEntityGroup>(entity));
        default:
            return entity->Instantiate(context);
        }
    }
}
