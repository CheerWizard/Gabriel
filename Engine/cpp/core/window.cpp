#include <core/window.h>

#include <io/image_loader.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windef.h>

#ifdef _WIN32

#pragma comment (lib, "Dwmapi")
#include <dwmapi.h>
#define winHandle glfwGetWin32Window(mHandle)

#endif

namespace gl {

    Window::Window(
            int x, int y,
            int width, int height,
            const char* title,
            bool enableFullscreen,
            bool enableVSync,
            int majorVersion, int minorVersion, int profileVersion
    ) :
            mX(x), mY(y),
            mWidth(width), mHeight(height),
            mTitle(title),
            mEnableFullscreen(enableFullscreen),
            mEnableVSync(enableVSync),
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

#ifdef IMGUI
        hideTitleBar();
#endif

        mHandle = glfwCreateWindow(width, height, title, null, null);
        if (!mHandle) {
            glfwTerminate();
            error("Failed to create GLFW window");
            assert(false);
        }

        setContext();
    }

    Window::~Window() {
        glfwDestroyWindow(mHandle);
        glfwTerminate();
    }

    void Window::setContext() {
        glfwMakeContextCurrent(mHandle);

        if (mEnableFullscreen)
            setFullScreen();
        else
            setWindowed();

        glfwSwapInterval(mEnableVSync);
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
        auto& refreshRate = mVideoModes[mPrimaryMonitor]->refreshRate;
        glfwSetWindowMonitor(mHandle, null, mX, mY, mWidth, mHeight, refreshRate);
    }

    void Window::setFullscreenWindowed() {
        mWindowModeX = mX;
        mWindowModeY = mY;
        mWindowModeWidth = mWidth;
        mWindowModeHeight = mHeight;
        auto& mode = mVideoModes[mPrimaryMonitor];
        glfwSetWindowMonitor(mHandle, null, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void Window::toggleWindowMode() {
        mEnableFullscreen = !mEnableFullscreen;
        if (mEnableFullscreen)
            setFullScreen();
        else
            setWindowed();
    }

    void Window::enableVSync() {
        glfwSwapInterval(true);
    }

    void Window::disableVSync() {
        glfwSwapInterval(false);
    }

    void Window::toggleVSync() {
        mEnableVSync = !mEnableVSync;
        if (mEnableVSync)
            enableVSync();
        else
            disableVSync();
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

    void Window::resizeFrame(int w, int h) {
        Viewport::resize(0, 0, w, h);
        mFrameWidth = w;
        mFrameHeight = h;
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

    void Window::onFrameResized(int w, int h) {
        mFrameWidth = w;
        mFrameHeight = h;
    }

    const char** Window::getExtensions(u32* count) {
        return glfwGetRequiredInstanceExtensions(count);
    }

    void Window::loadIcon(const char* filepath) {
        Image image = ImageReader::read(filepath);
        mIcon.width = image.width;
        mIcon.height = image.height;
        mIcon.pixels = (u8*) malloc(image.size());
        memcpy(mIcon.pixels, image.pixels, image.size());
        image.free();
        glfwSetWindowIcon(mHandle, 1, &mIcon);
    }

    void Window::setCursorMode(const CursorMode cursorMode) {
        if (mCurrentCursorMode != cursorMode) {
            mCurrentCursorMode = cursorMode;
            glfwSetInputMode(mHandle, GLFW_CURSOR, cursorMode);
        }
    }

    void Window::setTheme(const ThemeMode themeMode) {
#ifdef _WIN32
        BOOL value = TRUE;
        DwmSetWindowAttribute(winHandle, themeMode, &value, sizeof(value));
#endif
    }

    void Window::minimize() {
        glfwIconifyWindow(mHandle);
    }

    void Window::hideTitleBar() {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

}