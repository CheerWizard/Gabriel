#pragma once

#include <core/keycodes.h>
#include <api/device.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>

#include <unordered_map>

namespace gl {

    enum WindowFlags : u8 {
        None = 0,
        Sync = 1,
        Fullscreen = 2
    };

    struct Cursor final {
        double x = 0;
        double y = 0;
    };

    struct Window final {

        Window(
                int x, int y,
                int width, int height,
                const char* title, WindowFlags flags = WindowFlags::None,
                int majorVersion = 4, int minorVersion = 6, int profileVersion = GLFW_OPENGL_CORE_PROFILE
        );
        ~Window();

        [[nodiscard]] inline GLFWwindow* getHandle() const {
            return mHandle;
        }

        [[nodiscard]] inline int getX() const {
            return mX;
        }

        [[nodiscard]] inline int getY() const {
            return mY;
        }

        [[nodiscard]] inline int getWidth() const {
            return mWidth;
        }

        [[nodiscard]] inline int getHeight() const {
            return mHeight;
        }

        [[nodiscard]] inline const char* getTitle() const {
            return mTitle;
        }

        [[nodiscard]] inline float getAspectRatio() const {
            return (float) mWidth / (float) mHeight;
        }

        void setContext();

        void setFullScreen();
        void setWindowed();
        void toggleWindowMode();

        void enableSync();
        void disableSync();

        void poll();
        void swap();

        void close();
        bool isOpen();

        Cursor mouseCursor();
        void disableCursor();

        bool isKeyPress(int key);
        bool isKeyRelease(int key);

        bool isMousePress(int button);
        bool isMouseRelease(int key);

        void resize(int w, int h);
        void move(int x, int y);

        void getFrameSize(int& w, int& h);

        void onResized(int w, int h);
        void onMoved(int x, int y);

        static void setWindowErrorCallback(GLFWerrorfun errorFun);

        static const char** getExtensions(u32* count);

        template<typename T>
        void initCallbacks(void* callback);

        template<typename T>
        void setWindowCloseCallback();

        template<typename T>
        void setWindowResizeCallback();

        template<typename T>
        void setWindowMoveCallback();

        template<typename T>
        void setFramebufferResizeCallback();

        template<typename T>
        void setKeyCallback();

        template<typename T>
        void setMouseCallback();

        template<typename T>
        void setMouseCursorCallback();

        template<typename T>
        void setMouseScrollCallback();

    private:
        GLFWwindow* mHandle;

        const int mMajorVersion;
        const int mMinorVersion;
        const int mProfileVersion;

        const char* mTitle;

        int mX, mY;
        int mWidth, mHeight;

        int mWindowModeWidth, mWindowModeHeight;
        int mWindowModeX, mWindowModeY;

        GLFWmonitor* mPrimaryMonitor;
        std::unordered_map<GLFWmonitor*, const GLFWvidmode*> mVideoModes;

        WindowFlags mFlags;
        bool mEnableFullscreen = false;
    };

    template<typename T>
    void Window::initCallbacks(void* callback) {
        glfwSetWindowUserPointer(mHandle, callback);
    }

    template<typename T>
    void Window::setWindowCloseCallback() {
        glfwSetWindowCloseCallback(mHandle, [](GLFWwindow* win) {
            info("onWindowClose()")
            static_cast<T*>(glfwGetWindowUserPointer(win))->onWindowClose();
        });
    }

    template<typename T>
    void Window::setWindowResizeCallback() {
        glfwSetWindowSizeCallback(mHandle, [](GLFWwindow* win, int w, int h) {
            info("onWindowResize(" << w << ", " << h << ")")
            static_cast<T*>(glfwGetWindowUserPointer(win))->onWindowResize(w, h);
        });
    }

    template<typename T>
    void Window::setWindowMoveCallback() {
        glfwSetWindowPosCallback(mHandle, [](GLFWwindow* win, int x, int y) {
            info("onWindowPositioned(" << x << ", " << y << ")")
            if (x < 0 && y < 0) {
                error("Window: invalid position x < 0 or y < 0")
                return;
            }
            static_cast<T *>(glfwGetWindowUserPointer(win))->onWindowMove(x, y);
        });
    }

    template<typename T>
    void Window::setFramebufferResizeCallback() {
        glfwSetFramebufferSizeCallback(mHandle, [](GLFWwindow* win, int w, int h) {
            info("onFramebufferResized(" << w << ", " << h << ")")
            if (w <= 0 && h <= 0) {
                error("Window: invalid framebuffer size w <= 0 or h <= 0")
                return;
            }
            static_cast<T*>(glfwGetWindowUserPointer(win))->onFramebufferResized(w, h);
        });
    }

    template<typename T>
    void Window::setKeyCallback() {
        glfwSetKeyCallback(mHandle, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                static_cast<T*>(glfwGetWindowUserPointer(win))->onKeyPress(key);
            } else if (action == GLFW_RELEASE) {
                static_cast<T*>(glfwGetWindowUserPointer(win))->onKeyRelease(key);
            }
        });
    }

    template<typename T>
    void Window::setMouseCallback() {
        glfwSetMouseButtonCallback(mHandle, [](GLFWwindow* win, int button, int action, int mods) {
            if (action == GLFW_PRESS) {
                static_cast<T*>(glfwGetWindowUserPointer(win))->onMousePress(button);
            } else if (action == GLFW_RELEASE) {
                static_cast<T*>(glfwGetWindowUserPointer(win))->onMouseRelease(button);
            }
        });
    }

    template<typename T>
    void Window::setMouseCursorCallback() {
        glfwSetCursorPosCallback(mHandle, [](GLFWwindow* win, double x, double y) {
            static_cast<T*>(glfwGetWindowUserPointer(win))->onMouseCursor(x, y);
        });
    }

    template<typename T>
    void Window::setMouseScrollCallback() {
        glfwSetScrollCallback(mHandle, [](GLFWwindow* win, double x, double y) {
            static_cast<T*>(glfwGetWindowUserPointer(win))->onMouseScroll(x, y);
        });
    }
}