#include <imgui/entity_window.h>

#include <core/imgui_core.h>

namespace gl {

    const char* EntityWindow::title = "Entities";
    glm::vec2 EntityWindow::position = { 0, 0 };
    glm::vec2 EntityWindow::resolution = { 256, 256 };
    ImGuiWindowFlags EntityWindow::windowFlags = ImGuiWindowFlags_None;

    static bool pOpen = false;
    static bool pInitialized = false;

    void EntityWindow::render() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        if (!ImGui::Begin(title, &pOpen, windowFlags)) {
            end();
            return;
        }

        if (!pInitialized) {
            pInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
        }

        Scene* scene = ImguiCore::scene;

        if (scene) {

            for (EntityID entityId : scene->getEntities()) {
                if (entityId != InvalidEntity) {
                    Entity entity = { entityId, scene };
                    renderEntity(entity);
                }
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                ImguiCore::selectedEntity = {};

            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow("EntityPopupMenu")) {
                if (ImGui::MenuItem("Create Entity"))
                    ImguiCore::selectedEntity = Entity(scene);

                ImGui::EndPopup();
            }

        }

        end();
    }

    void EntityWindow::end() {
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    void EntityWindow::renderEntity(Entity entity) {
        EntityID id = entity.getId();
        auto& tag = entity.getComponent<Tag>()->buffer;

        ImGuiTreeNodeFlags flags = ImguiCore::selectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0;
        flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::TreeNodeEx((void*)(uint64_t)id, flags, "%s", tag.c_str());
        if (ImGui::IsItemClicked()) {
            ImguiCore::selectedEntity = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;

            ImGui::EndPopup();
        }

        if (entityDeleted) {
            if (ImguiCore::selectedEntity == entity) {
                ImguiCore::selectedEntity = {};
            }
            entity.free();
        }

        ImGui::TreePop();
    }

}