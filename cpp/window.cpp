#include <window.h>

#include <unordered_map>

namespace gl {

    static WindowProps win_props;
    static GLFWwindow* window;
    static GLFWmonitor* primary_monitor;
    static std::unordered_map<GLFWmonitor*, const GLFWvidmode*> video_modes;

    static int window_mode_width;
    static int window_mode_height;
    static int window_mode_x;
    static int window_mode_y;

    static bool enable_fullscreen = false;

    void Window::init(const WindowProps& props) {
        win_props = props;
        window_mode_x = props.x;
        window_mode_y = props.y;
        window_mode_width = props.width;
        window_mode_height = props.height;

        glfwSetErrorCallback([](int error, const char* msg) {
            print_err("GLFW error=" << error << ", msg=" << msg);
            if (EventRegistry::window_error) {
                EventRegistry::window_error(error, msg);
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

        Device::init(props.width, props.height);

        if (props.flags & WindowFlags::Fullscreen)
            set_full_screen();
        else
            set_windowed();

        glfwSwapInterval(props.flags & WindowFlags::Sync);
    }

    void Window::free() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::set_full_screen() {
        window_mode_x = win_props.x;
        window_mode_y = win_props.y;
        window_mode_width = win_props.width;
        window_mode_height = win_props.height;
        auto& mode = video_modes[primary_monitor];
        glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void Window::set_windowed() {
        win_props.x = window_mode_x;
        win_props.y = window_mode_y;
        win_props.width = window_mode_width;
        win_props.height = window_mode_height;
        auto& refresh_rate = video_modes[primary_monitor]->refreshRate;
        glfwSetWindowMonitor(window, null, win_props.x, win_props.y, win_props.width, win_props.height, refresh_rate);
    }

    void Window::toggle_window_mode() {
        enable_fullscreen = !enable_fullscreen;
        if (enable_fullscreen)
            set_full_screen();
        else
            set_windowed();
    }

    void Window::enable_sync() {
        glfwSwapInterval(true);
    }

    void Window::disable_sync() {
        glfwSwapInterval(false);
    }

    void Window::poll() {
        glfwPollEvents();
    }

    void Window::swap() {
        glfwSwapBuffers(window);
    }

    void Window::close() {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    bool Window::is_open() {
        return !glfwWindowShouldClose(window);
    }

    float Window::get_aspect_ratio() {
        return (float) win_props.width / (float) win_props.height;
    }

    WindowProps& Window::get_props() {
        return win_props;
    }

    int& Window::get_width() {
        return win_props.width;
    }

    int& Window::get_height() {
        return win_props.height;
    }

    Cursor Window::mouse_cursor() {
        Cursor cursor;
        glfwGetCursorPos(window, &cursor.x, &cursor.y);
        return cursor;
    }

    void Window::disable_cursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool Window::is_key_press(int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool Window::is_mouse_press(int button) {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    bool Window::is_key_release(int key) {
        return glfwGetKey(window, key) == GLFW_RELEASE;
    }

    bool Window::is_mouse_release(int button) {
        return glfwGetMouseButton(window, button) == GLFW_RELEASE;
    }

    EventWindowError EventRegistry::window_error = null;
    EventWindowResized EventRegistry::window_resized = null;
    EventWindowClosed EventRegistry::window_close = null;
    EventWindowPositioned EventRegistry::window_positioned = null;

    EventFramebufferResized EventRegistry::framebuffer_resized = null;

    EventKeyPress EventRegistry::key_press = null;
    EventKeyRelease EventRegistry::key_release = null;

    EventMousePress EventRegistry::mouse_press = null;
    EventMouseRelease EventRegistry::mouse_release = null;
    EventMouseCursor EventRegistry::mouse_cursor = null;
    EventMouseScroll EventRegistry::mouse_scroll = null;

    #define event_handler(event, ...) if (EventRegistry::event) { \
        EventRegistry::event(__VA_ARGS__);     \
    }

    void EventRegistry::set_callbacks() {
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
            Viewport::resize(0, 0, w, h);
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