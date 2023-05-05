#pragma once

#include <entity.h>
#include <polygon_visual.h>
#include <normal_visual.h>
#include <camera.h>
#include <terrain.h>

namespace gl {

    enum EntityControlFlag : u8 {
        Select = 1,
        Drag = 2,
        Hover = 4
    };

    component(Selectable) {
        typedef void (*entity_selected_callback) (ecs::Entity, double, double);

        entity_selected_callback callback = null;
        bool enable = false;

        Selectable() = default;
        Selectable(entity_selected_callback callback) : callback(callback) {}
    };

    component(Draggable) {
        typedef void (*entity_dragged_callback) (ecs::Entity, double, double);

        entity_dragged_callback callback = null;

        Draggable() = default;
        Draggable(entity_dragged_callback callback) : callback(callback) {}
    };

    component(Hoverable) {
        typedef void (*entity_hovered_callback) (ecs::Entity, double, double);

        entity_hovered_callback callback = null;

        Hoverable() = default;
        Hoverable(entity_hovered_callback callback) : callback(callback) {}
    };

    struct EntityControl final {
        ecs::Scene* scene;
        Camera* camera;
        EntityControlFlag flags = Select;

        EntityControl(ecs::Scene* scene, Camera* camera)
        : scene(scene), camera(camera) {}

        bool select(double x, double y);
        void drag(double x, double y);
        void hover(double x, double y);

    private:
        ecs::Entity mSelectedEntity;
    };

}