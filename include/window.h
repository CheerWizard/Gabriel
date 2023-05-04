#pragma once

#include <device.h>
#include <keycodes.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>

namespace gl {

    enum WindowFlags : u8 {
        Sync = 1,
        Fullscreen = 2
    };

    struct WindowProps final {
        int x, y;
        int width, height;
        const char* title;
        u8 flags = 0;
        int major_version = 4;
        int minor_version = 6;
        int profile_version = GLFW_OPENGL_CORE_PROFILE;
    };

    struct Cursor final {
        double x = 0;
        double y = 0;
    };

    struct Window final {
        static void init(const WindowProps& props);
        static void free();

        static void set_full_screen();
        static void set_windowed();
        static void toggle_window_mode();

        static void enable_sync();
        static void disable_sync();

        static void poll();
        static void swap();

        static void close();
        static bool is_open();

        static float get_aspect_ratio();
        static WindowProps& get_props();
        static int& get_width();
        static int& get_height();

        static Cursor mouse_cursor();
        static void disable_cursor();

        static bool is_key_press(int key);
        static bool is_key_release(int key);

        static bool is_mouse_press(int button);
        static bool is_mouse_release(int key);
    };

    typedef std::function<void(int, const char*)> EventWindowError;
    typedef std::function<void(int, int)> EventWindowResized;
    typedef std::function<void()> EventWindowClosed;
    typedef std::function<void(int, int)> EventWindowPositioned;

    typedef std::function<void(int, int)> EventFramebufferResized;

    typedef std::function<void(int)> EventKeyPress;
    typedef std::function<void(int)> EventKeyRelease;

    typedef std::function<void(int)> EventMousePress;
    typedef std::function<void(int)> EventMouseRelease;
    typedef std::function<void(double, double)> EventMouseCursor;
    typedef std::function<void(double, double)> EventMouseScroll;

    struct EventRegistry final {
        static EventWindowError window_error;
        static EventWindowResized window_resized;
        static EventWindowClosed window_close;
        static EventWindowPositioned window_positioned;

        static EventFramebufferResized framebuffer_resized;

        static EventKeyPress key_press;
        static EventKeyRelease key_release;

        static EventMousePress mouse_press;
        static EventMouseRelease mouse_release;
        static EventMouseCursor mouse_cursor;
        static EventMouseScroll mouse_scroll;

        static void set_callbacks();
    };

}