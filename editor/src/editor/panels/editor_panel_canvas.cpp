#include "common.h"
#include "editor_panel_canvas.h"
#include "app.h"
#include "editor/editor_layer.h"
#include "moth_ui/group.h"
#include "editor/bounds_widget.h"
#include "moth_ui/events/event_mouse.h"
#include "imgui_internal.h"
#include "../element_utils.h"
#include "moth_ui/layout/layout_entity_ref.h"
#include "moth_ui/layout/layout_entity_image.h"
#include "moth_ui/layout/layout.h"
#include "imgui_internal.h"

extern App* g_App;

EditorPanelCanvas::EditorPanelCanvas(EditorLayer& editorLayer, bool visible)
    : EditorPanel(editorLayer, "Canvas", visible, false)
    , m_boundsWidget(std::make_unique<BoundsWidget>(*this)) {
    LoadCanvasProperties();
}
void EditorPanelCanvas::OnShutdown() {
    SaveCanvasProperties();
}

bool EditorPanelCanvas::BeginPanel() {
    ImGui::SetNextWindowDockID(m_editorLayer.GetDockID());
    ImGuiWindowFlags host_window_flags = 0;
    host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool result = ImGui::Begin(m_title.c_str(), nullptr, host_window_flags);
    ImGui::PopStyleVar();
    return result;
}

void EditorPanelCanvas::DrawContents() {
    moth_ui::IntVec2 const windowRegionSize{ static_cast<int>(ImGui::GetContentRegionAvail().x), static_cast<int>(ImGui::GetContentRegionAvail().y) };

    UpdateDisplayTexture(*g_App->GetRenderer(), windowRegionSize);
    ImGui::Image(m_displayTexture.get(), ImVec2(static_cast<float>(windowRegionSize.x), static_cast<float>(windowRegionSize.y)));

    auto const drawList = ImGui::GetWindowDrawList();

    // draw selected item rects
    {
        auto const& selection = m_editorLayer.GetSelection();
        for (auto&& node : selection) {
            auto const rect = ConvertSpace<CoordSpace::WorldSpace, CoordSpace::AppSpace, float>(node->GetScreenRect());
            drawList->AddRect(ImVec2{ rect.topLeft.x, rect.topLeft.y }, ImVec2{ rect.bottomRight.x, rect.bottomRight.y }, 0xFFFF00FF);
        }
    }

    // draw the selection rect if we have one
    {
        if (m_dragSelecting) {
            auto const minX = static_cast<float>(std::min(m_dragSelectStart.x, m_dragSelectEnd.x));
            auto const maxX = static_cast<float>(std::max(m_dragSelectStart.x, m_dragSelectEnd.x));
            auto const minY = static_cast<float>(std::min(m_dragSelectStart.y, m_dragSelectEnd.y));
            auto const maxY = static_cast<float>(std::max(m_dragSelectStart.y, m_dragSelectEnd.y));
            if ((maxX - minX) > 3 || (maxY - minY) > 3) {
                drawList->AddRect(ImVec2{ minX, minY }, ImVec2{ maxX, maxY }, 0xFFFFFFFF);
            }
        }
    }

    auto const selection = m_editorLayer.GetSelection();
    auto const firstIt = std::begin(selection);
    if (selection.size() == 1 && !m_editorLayer.IsLocked(*firstIt)) {
        m_boundsWidget->SetSelection(*firstIt);
    } else {
        m_boundsWidget->SetSelection(nullptr);
    }
    m_boundsWidget->Draw(*g_App->GetRenderer());

    UpdateInput();
}

void EditorPanelCanvas::UpdateDisplayTexture(SDL_Renderer& renderer, moth_ui::IntVec2 const& windowSize) {
    if (!m_displayTexture || m_canvasWindowSize != windowSize) {
        m_displayTexture = CreateTextureRef(SDL_CreateTexture(g_App->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowSize.x, windowSize.y));
    }

    m_canvasWindowPos = moth_ui::IntVec2{ static_cast<int>(ImGui::GetWindowPos().x), static_cast<int>(ImGui::GetWindowPos().y) };
    m_canvasWindowSize = windowSize;

    auto const oldRenderTarget = SDL_GetRenderTarget(&renderer);
    SDL_SetRenderTarget(&renderer, m_displayTexture.get());
    SDL_SetRenderDrawColor(&renderer, 0xAA, 0xAA, 0xAA, 0xFF);
    SDL_RenderClear(&renderer);

    // clear the canvas area
    {
        SDL_SetRenderDrawColor(&renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        moth_ui::IntRect canvasRect;
        canvasRect.topLeft = { 0, 0 };
        canvasRect.bottomRight = m_canvasSize;
        auto const sdlRectF = ToSDL(ConvertSpace<CoordSpace::CanvasSpace, CoordSpace::WorldSpace, float>(canvasRect));
        SDL_RenderFillRectF(&renderer, &sdlRectF);
    }

    // grid lines
    {
        SDL_SetRenderDrawColor(&renderer, 0xDD, 0xDD, 0xDD, 0xFF);
        if (m_canvasGridSpacing > 0) {
            for (int x = m_canvasGridSpacing; x < m_canvasSize.x; x += m_canvasGridSpacing) {
                moth_ui::IntVec2 const p0{ x, 0 };
                moth_ui::IntVec2 const p1{ x, m_canvasSize.y };
                auto const p0Scaled = ConvertSpace<CoordSpace::CanvasSpace, CoordSpace::WorldSpace, float>(p0);
                auto const p1Scaled = ConvertSpace<CoordSpace::CanvasSpace, CoordSpace::WorldSpace, float>(p1);
                SDL_RenderDrawLineF(&renderer, p0Scaled.x, p0Scaled.y, p1Scaled.x, p1Scaled.y);
            }
            for (int y = m_canvasGridSpacing; y < m_canvasSize.y; y += m_canvasGridSpacing) {
                moth_ui::IntVec2 const p0{ 0, y };
                moth_ui::IntVec2 const p1{ m_canvasSize.x, y };
                auto const p0Scaled = ConvertSpace<CoordSpace::CanvasSpace, CoordSpace::WorldSpace, float>(p0);
                auto const p1Scaled = ConvertSpace<CoordSpace::CanvasSpace, CoordSpace::WorldSpace, float>(p1);
                SDL_RenderDrawLineF(&renderer, p0Scaled.x, p0Scaled.y, p1Scaled.x, p1Scaled.y);
            }
        }
    }

    // setup scaling and draw the layout
    {
        auto const scaleFactor = m_canvasZoom / 100.0f;
        auto const newRenderWidth = static_cast<int>(windowSize.x / scaleFactor);
        auto const newRenderHeight = static_cast<int>(windowSize.y / scaleFactor);
        auto const newRenderOffsetX = static_cast<int>(m_canvasOffset.x / scaleFactor);
        auto const newRenderOffsetY = static_cast<int>(m_canvasOffset.y / scaleFactor);

        SDL_RenderSetLogicalSize(&renderer, newRenderWidth, newRenderHeight);
        {
            SDL_Rect const guideRect{
                newRenderOffsetX + (newRenderWidth - m_canvasSize.x) / 2,
                newRenderOffsetY + (newRenderHeight - m_canvasSize.y) / 2,
                m_canvasSize.x,
                m_canvasSize.y
            };
            if (auto const root = m_editorLayer.GetRoot()) {
                moth_ui::IntRect displayRect;
                displayRect.topLeft = { guideRect.x, guideRect.y };
                displayRect.bottomRight = { guideRect.x + guideRect.w, guideRect.y + guideRect.h };
                root->SetScreenRect(displayRect);
                root->Draw();
            }
        }
        SDL_RenderSetLogicalSize(&renderer, m_canvasWindowSize.x, m_canvasWindowSize.y); // reset logical sizing
    }

    SDL_SetRenderTarget(&renderer, oldRenderTarget);
}

void EditorPanelCanvas::EndPanel() {
    auto const windowPos = ImGui::GetWindowPos();
    auto const windowSize = ImGui::GetWindowSize();
    ImRect const windowContentRect{ windowPos + ImVec2{ 5, 5 }, windowPos + windowSize - ImVec2{ 5, 5 } };
    auto const windowID = ImGui::GetCurrentWindow()->ID;

    if (ImGui::BeginDragDropTargetCustom(windowContentRect, windowID)) {
        if (auto const payload = ImGui::AcceptDragDropPayload("layout_path", 0)) {
            std::string* layoutPath = static_cast<std::string*>(payload->Data);
            std::shared_ptr<moth_ui::Layout> newLayout;
            auto loadResult = moth_ui::Layout::Load(layoutPath->c_str(), &newLayout);
            if (loadResult == moth_ui::Layout::LoadResult::Success) {
                AddEntity<moth_ui::LayoutEntityRef>(m_editorLayer, *newLayout);
            }
        } else if (auto const payload = ImGui::AcceptDragDropPayload("image_path", 0)) {
            std::string* layoutPath = static_cast<std::string*>(payload->Data);
            AddEntity<moth_ui::LayoutEntityImage>(m_editorLayer, layoutPath->c_str());
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

moth_ui::IntVec2 EditorPanelCanvas::SnapToGrid(moth_ui::IntVec2 const& original) {
    if (m_canvasGridSpacing > 0) {
        float const s = static_cast<float>(m_canvasGridSpacing);
        int const x = static_cast<int>(std::round(original.x / s) * s);
        int const y = static_cast<int>(std::round(original.y / s) * s);
        return { x, y };
    } else {
        return original;
    }
}

void EditorPanelCanvas::OnMouseClicked(moth_ui::IntVec2 const& appPosition) {
    bool handled = m_boundsWidget->OnEvent(moth_ui::EventMouseDown(moth_ui::MouseButton::Left, appPosition));

    if (!m_holdingSelection) {
        auto const worldPosition = ConvertSpace<CoordSpace::AppSpace, CoordSpace::WorldSpace, int>(appPosition);
        auto const selection = m_editorLayer.GetSelection();
        for (auto&& node : selection) {
            if (node->IsInBounds(worldPosition)) {
                m_holdingSelection = true;
                m_grabPosition = SnapToGrid(worldPosition);
                m_editorLayer.BeginEditBounds();
                //handled = true;
                break;
            }
        }
    }

    if (!handled && !m_dragSelecting) {
        m_dragSelecting = true;
        m_dragSelectStart = appPosition;
        m_dragSelectEnd = appPosition;
    }
}

void EditorPanelCanvas::OnMouseReleased(moth_ui::IntVec2 const& appPosition) {
    bool handled = m_boundsWidget->OnEvent(moth_ui::EventMouseUp(moth_ui::MouseButton::Left, appPosition));

    if (m_holdingSelection) {
        m_editorLayer.EndEditBounds();
        m_holdingSelection = false;
    }

    if (!handled && m_dragSelecting) {
        m_dragSelectEnd = appPosition;

        auto const minX = std::min(m_dragSelectStart.x, m_dragSelectEnd.x);
        auto const maxX = std::max(m_dragSelectStart.x, m_dragSelectEnd.x);
        auto const minY = std::min(m_dragSelectStart.y, m_dragSelectEnd.y);
        auto const maxY = std::max(m_dragSelectStart.y, m_dragSelectEnd.y);

        moth_ui::IntRect selectionRect;
        selectionRect.topLeft = ConvertSpace<CoordSpace::AppSpace, CoordSpace::WorldSpace, int>(moth_ui::IntVec2{ minX, minY });
        selectionRect.bottomRight = ConvertSpace<CoordSpace::AppSpace, CoordSpace::WorldSpace, int>(moth_ui::IntVec2{ maxX, maxY });

        if (!ImGui::GetIO().KeyCtrl) {
            m_editorLayer.ClearSelection();
        }
        SelectInRect(selectionRect);
    }

    m_dragSelecting = false;
}

void EditorPanelCanvas::OnMouseMoved(moth_ui::IntVec2 const& appPosition) {
    bool handled = m_boundsWidget->OnEvent(moth_ui::EventMouseMove(appPosition, static_cast<moth_ui::FloatVec2>(appPosition - m_lastMousePos)));

    if (m_holdingSelection) {
        auto const worldPosition = ConvertSpace<CoordSpace::AppSpace, CoordSpace::WorldSpace, int>(appPosition);
        auto const newPosition = SnapToGrid(worldPosition);
        auto const delta = newPosition - m_grabPosition;
        m_grabPosition = newPosition;

        auto const selection = m_editorLayer.GetSelection();
        for (auto&& node : selection) {
            auto& bounds = node->GetLayoutRect();
            bounds.offset.topLeft += static_cast<moth_ui::FloatVec2>(delta);
            bounds.offset.bottomRight += static_cast<moth_ui::FloatVec2>(delta);
            node->RecalculateBounds();
        }
        m_dragSelecting = false;
    }

    if (m_dragSelecting) {
        m_dragSelectEnd = appPosition;
    }
}

void EditorPanelCanvas::SelectInRect(moth_ui::IntRect const& selectionRect) {
    bool const single = selectionRect.w() <= 3 || selectionRect.h() <= 3;
    auto const& children = m_editorLayer.GetRoot()->GetChildren();
    for (auto it = std::rbegin(children); it != std::rend(children); ++it) {
        auto const& child = *it;
        if (child->IsVisible() && !m_editorLayer.IsLocked(child)) {
            auto const& screenRect = child->GetScreenRect();
            if (moth_ui::Intersects(selectionRect, screenRect)) {
                if (single && m_editorLayer.IsSelected(child)) {
                    m_editorLayer.RemoveSelection(child);
                } else {
                    m_editorLayer.AddSelection(child);
                }
                if (single) {
                    break;
                }
            }
        }
    }
}

void EditorPanelCanvas::UpdateInput() {
    auto const mousePos = moth_ui::IntVec2{ ImGui::GetMousePos().x, ImGui::GetMousePos().y };

    if (ImGui::IsWindowHovered()) {
        float const scaleFactor = m_canvasZoom / 100.0f;

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            if (!m_draggingCanvas) {
                m_draggingCanvas = true;
                m_initialCanvasOffset = m_canvasOffset;
            }

            auto const dragDelta = moth_ui::FloatVec2{ ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle).x, ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle).y };

            m_canvasOffset = m_initialCanvasOffset + (moth_ui::FloatVec2{ dragDelta.x, dragDelta.y });
        } else {
            m_draggingCanvas = false;
        }

        auto const io = ImGui::GetIO();
        if (io.MouseWheel != 0) {
            m_canvasZoom += static_cast<int>(io.MouseWheel * 6 * scaleFactor);
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            OnMouseClicked(moth_ui::IntVec2{ mousePos.x, mousePos.y });
        }
        if (m_lastMousePos != mousePos) {
            OnMouseMoved(moth_ui::IntVec2{ mousePos.x, mousePos.y });
            m_lastMousePos = mousePos;
        }
    }

    if (ImGui::IsWindowFocused()) {
        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            m_editorLayer.DeleteEntity();
        }
    }

    // always want to accept released so we dont end up stuck down
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        OnMouseReleased(moth_ui::IntVec2{ mousePos.x, mousePos.y });
    }
}

void EditorPanelCanvas::LoadCanvasProperties() {
    auto const& persistenceJson = g_App->GetPersistentState();
    if (persistenceJson.contains("canvas_properties")) {
        auto const& canvasJson = persistenceJson["canvas_properties"];
        m_canvasSize = canvasJson.value("size", m_canvasSize);
        m_canvasGridSpacing = canvasJson.value("grid_spacing", m_canvasGridSpacing);
    }
}

void EditorPanelCanvas::SaveCanvasProperties() {
    auto& persistenceJson = g_App->GetPersistentState();
    nlohmann::json canvasJson;
    canvasJson["size"] = m_canvasSize;
    canvasJson["grid_spacing"] = m_canvasGridSpacing;
    persistenceJson["canvas_properties"] = canvasJson;
}
