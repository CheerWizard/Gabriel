#include <window.h>

#include <glad/glad.h>

namespace win {

    static window_props s_props;
    static GLFWwindow* s_win;

    int gpu_props::max_attrs_allowed;

    void init(const window_props& props) {
        int status = glfwInit();
        if (status == GLFW_FALSE) {
            print_err("Failed to initialize GLFW");
            assert(false);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, props.major_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, props.minor_version);
        glfwWindowHint(GLFW_OPENGL_PROFILE, props.profile_version);

        s_win = glfwCreateWindow(props.width, props.height, props.title, null, null);
        if (!s_win) {
            glfwTerminate();
            print_err("Failed to create GLFW window");
            assert(false);
        }

        glfwMakeContextCurrent(s_win);

        int glStatus = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (glStatus == GLFW_FALSE) {
            free();
            print_err("Failed to initialize GLAD");
            assert(false);
        }

        s_props = props;

        glfwSwapInterval(props.sync);

        glViewport(0, 0, props.width, props.height);

        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gpu_props::max_attrs_allowed);
    }

    void free() {
        glfwDestroyWindow(s_win);
        glfwTerminate();
    }

    void update() {
        glfwSwapBuffers(s_win);
        glfwPollEvents();
    }

    void close() {
        glfwSetWindowShouldClose(s_win, GLFW_TRUE);
    }

    bool is_open() {
        return !glfwWindowShouldClose(s_win);
    }

    float get_aspect_ratio() {
        return (float) s_props.width / (float) s_props.height;
    }

    window_props& props() {
        return s_props;
    }

    void disable_cursor() {
        glfwSetInputMode(s_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool is_key_press(int key) {
        return glfwGetKey(s_win, key) == GLFW_PRESS;
    }

    bool is_mouse_press(int button) {
        return glfwGetMouseButton(s_win, button) == GLFW_PRESS;
    }

    bool is_key_release(int key) {
        return glfwGetKey(s_win, key) == GLFW_RELEASE;
    }

    bool is_mouse_release(int button) {
        return glfwGetMouseButton(s_win, button) == GLFW_RELEASE;
    }

    event_window_resized event_registry::window_resized = null;
    event_window_close event_registry::window_close = null;

    event_framebuffer_resized event_registry::framebuffer_resized = null;

    event_key_press event_registry::key_press = null;
    event_key_release event_registry::key_release = null;

    event_mouse_press event_registry::mouse_press = null;
    event_mouse_release event_registry::mouse_release = null;
    event_mouse_cursor event_registry::mouse_cursor = null;
    event_mouse_scroll event_registry::mouse_scroll = null;

    #define event_handler(event, ...) if (event_registry::event) { \
        event_registry::event(__VA_ARGS__);                                                                \
    }

    void event_registry_update() {
        glfwSetWindowSizeCallback(s_win, [](GLFWwindow* win, int w, int h) {
            s_props.width = w;
            s_props.height = h;
            event_handler(window_resized, w, h)
        });
        glfwSetWindowCloseCallback(s_win, [](GLFWwindow* win) {
            event_handler(window_close)
        });

        glfwSetFramebufferSizeCallback(s_win, [](GLFWwindow* win, int w, int h) {
            glViewport(0, 0, w, h);
            event_handler(framebuffer_resized, w, h)
        });

        glfwSetKeyCallback(s_win, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                event_handler(key_press, key)
            } else if (action == GLFW_RELEASE) {
                event_handler(key_release, key)
            }
        });

        glfwSetMouseButtonCallback(s_win, [](GLFWwindow* win, int button, int action, int mods) {
            if (action == GLFW_PRESS) {
                event_handler(mouse_press, button)
            } else if (action == GLFW_RELEASE) {
                event_handler(mouse_release, button)
            }
        });

        glfwSetCursorPosCallback(s_win, [](GLFWwindow* win, double x, double y) {
            event_handler(mouse_cursor, x, y)
        });

        glfwSetScrollCallback(s_win, [](GLFWwindow* win, double x, double y) {
            event_handler(mouse_scroll, x, y)
        });
    }

}