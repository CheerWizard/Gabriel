#pragma once

#include <debugging/polygon_visual.h>
#include <debugging/normal_visual.h>

#include <control/camera.h>

#include <terrain/terrain.h>

namespace gl {

    enum EntityControlFlag : u8 {
        Select = 1,
        Drag = 2,
        Hover = 4
    };

    component(Selectable) {
        typedef void (*entity_selected_callback) (Entity, double, double);

        entity_selected_callback callback = null;
        bool enable = false;

        Selectable() = default;
        Selectable(entity_selected_callback callback) : callback(callback) {}
    };

    component(Draggable) {
        typedef void (*entity_dragged_callback) (Entity, double, double);

        entity_dragged_callback callback = null;

        Draggable() = default;
        Draggable(entity_dragged_callback callback) : callback(callback) {}
    };

    component(Hoverable) {
        typedef void (*entity_hovered_callback) (Entity, double, double);

        entity_hovered_callback callback = null;

        Hoverable() = default;
        Hoverable(entity_hovered_callback callback) : callback(callback) {}
    };

    struct EntityControl final {
        Scene* scene;
        Camera* camera;
        EntityControlFlag flags = Select;

        EntityControl(Scene* scene, Camera* camera)
        : scene(scene), camera(camera) {}

        bool select(double x, double y);
        void drag(double x, double y);
        void hover(double x, double y);

    private:
        Entity mSelectedEntity;
    };

}