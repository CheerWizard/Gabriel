#include <core/window.h>

#include <unordered_map>

namespace gl {

    Window::Window(
            int x, int y,
            int width, int height,
            const char* title, WindowFlags flags,
            int majorVersion, int minorVersion, int profileVersion
    ) :
    mX(x), mY(y),
    mWidth(width), mHeight(height),
    mTitle(title), mFlags(flags),
    mWindowModeX(x), mWindowModeY(y),
    mWindowModeWidth(width), mWindowModeHeight(height),
    mMajorVersion(majorVersion), mMinorVersion(minorVersion), mProfileVersion(profileVersion)
    {

        int status = glfwInit();
        if (status == GLFW_FALSE) {
            error("Failed to initialize GLFW");
            assert(false);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, profileVersion);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        mPrimaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* primary_mode = glfwGetVideoMode(mPrimaryMonitor);
        mWindowModeX = primary_mode->width / 4;
        mWindowModeY = primary_mode->height / 4;
        mVideoModes[mPrimaryMonitor] = primary_mode;

        glfwWindowHint(GLFW_RED_BITS, primary_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, primary_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, primary_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, primary_mode->refreshRate);

        mHandle = glfwCreateWindow(width, height, title, null, null);
        if (!mHandle) {
            glfwTerminate();
            error("Failed to create GLFW window");
            assert(false);
        }

        glfwMakeContextCurrent(mHandle);

        if (flags & WindowFlags::Fullscreen)
            setFullScreen();
        else
            setWindowed();

        glfwSwapInterval(flags & WindowFlags::Sync);
    }

    Window::~Window() {
        glfwDestroyWindow(mHandle);
        glfwTerminate();
    }

    void Window::setContext() {
        glfwMakeContextCurrent(mHandle);

        if (mFlags & WindowFlags::Fullscreen)
            setFullScreen();
        else
            setWindowed();

        glfwSwapInterval(mFlags & WindowFlags::Sync);
    }

    void Window::setFullScreen() {
        mWindowModeX = mX;
        mWindowModeY = mY;
        mWindowModeWidth = mWidth;
        mWindowModeHeight = mHeight;
        auto& mode = mVideoModes[mPrimaryMonitor];
        glfwSetWindowMonitor(mHandle, mPrimaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void Window::setWindowed() {
        mX = mWindowModeX;
        mY = mWindowModeY;
        mWidth = mWindowModeWidth;
        mHeight = mWindowModeHeight;
        auto& refresh_rate = mVideoModes[mPrimaryMonitor]->refreshRate;
        glfwSetWindowMonitor(mHandle, null, mX, mY, mWidth, mHeight, refresh_rate);
    }

    void Window::toggleWindowMode() {
        mEnableFullscreen = !mEnableFullscreen;
        if (mEnableFullscreen)
            setFullScreen();
        else
            setWindowed();
    }

    void Window::enableSync() {
        glfwSwapInterval(true);
    }

    void Window::disableSync() {
        glfwSwapInterval(false);
    }

    void Window::poll() {
        glfwPollEvents();
    }

    void Window::swap() {
        glfwSwapBuffers(mHandle);
    }

    void Window::close() {
        glfwSetWindowShouldClose(mHandle, GLFW_TRUE);
    }

    bool Window::isOpen() {
        return !glfwWindowShouldClose(mHandle);
    }

    Cursor Window::mouseCursor() {
        Cursor cursor;
        glfwGetCursorPos(mHandle, &cursor.x, &cursor.y);
        return cursor;
    }

    void Window::disableCursor() {
        glfwSetInputMode(mHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool Window::isKeyPress(int key) {
        return glfwGetKey(mHandle, key) == GLFW_PRESS;
    }

    bool Window::isMousePress(int button) {
        return glfwGetMouseButton(mHandle, button) == GLFW_PRESS;
    }

    bool Window::isKeyRelease(int key) {
        return glfwGetKey(mHandle, key) == GLFW_RELEASE;
    }

    bool Window::isMouseRelease(int button) {
        return glfwGetMouseButton(mHandle, button) == GLFW_RELEASE;
    }

    void Window::setWindowErrorCallback(GLFWerrorfun errorFun) {
        glfwSetErrorCallback(errorFun);
    }

    void Window::resize(int w, int h) {
        glfwSetWindowSize(mHandle, w, h);
    }

    void Window::move(int x, int y) {
        glfwSetWindowPos(mHandle, x, y);
    }

    void Window::getFrameSize(int& w, int& h) {
        glfwGetFramebufferSize(mHandle, &w, &h);
    }

    void Window::onResized(int w, int h) {
        mWidth = w;
        mHeight = h;
    }

    void Window::onMoved(int x, int y) {
        mX = x;
        mY = y;
    }

    const char** Window::getExtensions(u32* count) {
        return glfwGetRequiredInstanceExtensions(count);
    }

}