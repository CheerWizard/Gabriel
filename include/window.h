#pragma once

#include <keycodes.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace win {

    struct window_props final {
        int width, height;
        const char* title;
        bool sync = false;
        int major_version = 4;
        int minor_version = 2;
        int profile_version = GLFW_OPENGL_CORE_PROFILE;
    };

    void init(const window_props& props);
    void free();

    void update();

    void close();
    bool is_open();

    float get_aspect_ratio();
    window_props& props();

    void disable_cursor();

    bool is_key_press(int key);
    bool is_key_release(int key);

    bool is_mouse_press(int button);
    bool is_mouse_release(int key);

    typedef void (*event_window_resized)(int, int);
    typedef void (*event_window_close)();

    typedef void (*event_framebuffer_resized)(int, int);

    typedef void (*event_key_press)(int);
    typedef void (*event_key_release)(int);

    typedef void (*event_mouse_press)(int);
    typedef void (*event_mouse_release)(int);
    typedef void (*event_mouse_cursor)(double, double);
    typedef void (*event_mouse_scroll)(double, double);

    struct event_registry final {
        static event_window_resized window_resized;
        static event_window_close window_close;

        static event_framebuffer_resized framebuffer_resized;

        static event_key_press key_press;
        static event_key_release key_release;

        static event_mouse_press mouse_press;
        static event_mouse_release mouse_release;
        static event_mouse_cursor mouse_cursor;
        static event_mouse_scroll mouse_scroll;
    };

    void event_registry_update();

    struct gpu_props final {
        static int max_attrs_allowed;
    };

}