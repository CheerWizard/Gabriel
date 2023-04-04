#include <window.h>
#include <ui.h>

#include <unordered_map>

namespace win {

    static window_props win_props;
    static GLFWwindow* window;
    static GLFWmonitor* primary_monitor;
    static std::unordered_map<GLFWmonitor*, const GLFWvidmode*> video_modes;

    static int window_mode_width;
    static int window_mode_height;
    static int window_mode_x;
    static int window_mode_y;

    static bool enable_fullscreen = false;

    void init(const window_props& props) {
        win_props = props;
        window_mode_x = props.x;
        window_mode_y = props.y;
        window_mode_width = props.width;
        window_mode_height = props.height;

        glfwSetErrorCallback([](int error, const char* msg) {
            print_err("GLFW error=" << error << ", msg=" << msg);
            if (event_registry::window_error) {
                event_registry::window_error(error, msg);
            }
        });

        int status = glfwInit();
        if (status == GLFW_FALSE) {
            print_err("Failed to initialize GLFW");
            assert(false);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, props.major_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, props.minor_version);
        glfwWindowHint(GLFW_OPENGL_PROFILE, props.profile_version);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        primary_monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* primary_mode = glfwGetVideoMode(primary_monitor);
        window_mode_x = primary_mode->width / 4;
        window_mode_y = primary_mode->height / 4;
        video_modes[primary_monitor] = primary_mode;

        glfwWindowHint(GLFW_RED_BITS, primary_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, primary_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, primary_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, primary_mode->refreshRate);

        window = glfwCreateWindow(props.width, props.height, props.title, null, null);
        if (!window) {
            glfwTerminate();
            print_err("Failed to create GLFW window");
            assert(false);
        }

        glfwMakeContextCurrent(window);

        gl::init(props.width, props.height);

        if (props.flags & win_flags::fullscreen)
            set_full_screen();
        else
            set_windowed();

        glfwSwapInterval(props.flags & win_flags::sync);

#ifdef UI
        ui::init(window);
        disable_cursor();
#endif
    }

    void free() {
#ifdef UI
        ui::free();
#else
        glfwDestroyWindow(window);
#endif
        glfwTerminate();
    }

    void set_full_screen() {
        window_mode_x = win_props.x;
        window_mode_y = win_props.y;
        window_mode_width = win_props.width;
        window_mode_height = win_props.height;
        auto& mode = video_modes[primary_monitor];
        glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void set_windowed() {
        win_props.x = window_mode_x;
        win_props.y = window_mode_y;
        win_props.width = window_mode_width;
        win_props.height = window_mode_height;
        auto& refresh_rate = video_modes[primary_monitor]->refreshRate;
        glfwSetWindowMonitor(window, null, win_props.x, win_props.y, win_props.width, win_props.height, refresh_rate);
    }

    void toggle_window_mode() {
        enable_fullscreen = !enable_fullscreen;
        if (enable_fullscreen)
            win::set_full_screen();
        else
            win::set_windowed();
    }

    void enable_sync() {
        glfwSwapInterval(true);
    }

    void disable_sync() {
        glfwSwapInterval(false);
    }

    void poll() {
        glfwPollEvents();
    }

    void swap() {
        glfwSwapBuffers(window);
    }

    void close() {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    bool is_open() {
        return !glfwWindowShouldClose(window);
    }

    float get_aspect_ratio() {
        return (float) win_props.width / (float) win_props.height;
    }

    window_props& props() {
        return win_props;
    }

    void disable_cursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool is_key_press(int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool is_mouse_press(int button) {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    bool is_key_release(int key) {
        return glfwGetKey(window, key) == GLFW_RELEASE;
    }

    bool is_mouse_release(int button) {
        return glfwGetMouseButton(window, button) == GLFW_RELEASE;
    }

    event_window_error event_registry::window_error = null;
    event_window_resized event_registry::window_resized = null;
    event_window_close event_registry::window_close = null;
    event_window_positioned event_registry::window_positioned = null;

    event_framebuffer_resized event_registry::framebuffer_resized = null;

    event_key_press event_registry::key_press = null;
    event_key_release event_registry::key_release = null;

    event_mouse_press event_registry::mouse_press = null;
    event_mouse_release event_registry::mouse_release = null;
    event_mouse_cursor event_registry::mouse_cursor = null;
    event_mouse_scroll event_registry::mouse_scroll = null;

    #define event_handler(event, ...) if (event_registry::event) { \
        event_registry::event(__VA_ARGS__);     \
    }

    void event_registry::set_callbacks() {
        glfwSetErrorCallback([](int error, const char* msg) {
            event_handler(window_error, error, msg)
        });

        glfwSetWindowCloseCallback(window, [](GLFWwindow* win) {
            event_handler(window_close)
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int w, int h) {
            win_props.width = w;
            win_props.height = h;
            event_handler(window_resized, w, h)
        });

        glfwSetWindowPosCallback(window, [](GLFWwindow* win, int x, int y) {
            if (x < 0 && y < 0) {
                print_err("Window: invalid position x < 0 or y < 0");
                return;
            }
            win_props.x = x;
            win_props.y = y;
            event_handler(window_positioned, x, y)
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
            if (w <= 0 && h <= 0) {
                print_err("Window: invalid framebuffer size w <= 0 or h <= 0");
                return;
            }
            gl::resize(w, h);
            event_handler(framebuffer_resized, w, h)
        });

        glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                event_handler(key_press, key)
            } else if (action == GLFW_RELEASE) {
                event_handler(key_release, key)
            }
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
            if (action == GLFW_PRESS) {
                event_handler(mouse_press, button)
            } else if (action == GLFW_RELEASE) {
                event_handler(mouse_release, button)
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
            event_handler(mouse_cursor, x, y)
        });

        glfwSetScrollCallback(window, [](GLFWwindow* win, double x, double y) {
            event_handler(mouse_scroll, x, y)
        });
    }

}