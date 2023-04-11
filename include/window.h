#pragma once

#include <device.h>
#include <keycodes.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace win {

    enum win_flags : u8 {
        none = 0,
        sync = 1,
        fullscreen = 2
    };

    struct window_props final {
        int x, y;
        int width, height;
        const char* title;
        u8 flags = win_flags::none;
        int major_version = 4;
        int minor_version = 6;
        int profile_version = GLFW_OPENGL_CORE_PROFILE;
    };

    struct Cursor final {
        double x = 0;
        double y = 0;
    };

    void init(const window_props& props);
    void free();

    void set_full_screen();
    void set_windowed();
    void toggle_window_mode();

    void enable_sync();
    void disable_sync();

    void poll();
    void swap();

    void close();
    bool is_open();

    float get_aspect_ratio();
    window_props& props();

    Cursor mouse_cursor();
    void disable_cursor();

    bool is_key_press(int key);
    bool is_key_release(int key);

    bool is_mouse_press(int button);
    bool is_mouse_release(int key);

    typedef void (*event_window_error)(int, const char*);
    typedef void (*event_window_resized)(int, int);
    typedef void (*event_window_close)();
    typedef void (*event_window_positioned)(int, int);

    typedef void (*event_framebuffer_resized)(int, int);

    typedef void (*event_key_press)(int);
    typedef void (*event_key_release)(int);

    typedef void (*event_mouse_press)(int);
    typedef void (*event_mouse_release)(int);
    typedef void (*event_mouse_cursor)(double, double);
    typedef void (*event_mouse_scroll)(double, double);

    struct event_registry final {
        static event_window_error window_error;
        static event_window_resized window_resized;
        static event_window_close window_close;
        static event_window_positioned window_positioned;

        static event_framebuffer_resized framebuffer_resized;

        static event_key_press key_press;
        static event_key_release key_release;

        static event_mouse_press mouse_press;
        static event_mouse_release mouse_release;
        static event_mouse_cursor mouse_cursor;
        static event_mouse_scroll mouse_scroll;

        static void set_callbacks();
    };

}