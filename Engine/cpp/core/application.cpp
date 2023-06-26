#include <core/application.h>

namespace gl {

    static void onWindowError(int code, const char* message) {
        error("Error code: {0}\nError message: {1}", code, message);
    }

    Application::Application(const char* title, int width, int height, const char* logoName, const ThemeMode themeMode)
    : mTitle(title),
    mWidth(width), mHeight(height),
    mLogoName(logoName),
    mThemeMode(themeMode)
    {}

    Application::~Application() {}

    void Application::run() {
        onCreate();

        mRunning = true;
        float dt = 6;

        while (mRunning) {
            Timer::begin();

            mRunning = mWindow->isOpen();

            if (mScene) {

                mWindow->poll();

                onUpdateInput(dt);

#ifdef IMGUI
                if (enableSimulation) {
                    onSimulate(dt);
                }
#else
                onSimulate(Timer::getDeltaMillis());
#endif

                onRender(dt);

            }

            mWindow->swap();

            Timer::end();

            printDt();

            dt = Timer::getDeltaMillis();
        }

        onDestroy();
    }

    void Application::onCreate() {

#ifdef DEBUG
        initLogger();
#endif

        initWindow();

        initApi();

        initNetwork();

#ifdef IMGUI
        initImgui();
#endif

    }

    void Application::onCreateImgui() {}

    void Application::onDestroy() {
        delete mDatabaseService;
        delete mTcpClient;
        NetworkCore::free();

        delete mOutlineRenderer;

        delete mEnvRenderer;

        delete mTransparentRenderer;

        delete mRayTraceRenderer;

        EnvStorage::free();

        LightStorage::free();

        FontAtlas::free();

        delete mScreenRenderer;

        delete mPbrPipeline;
        delete mUiPipeline;
        delete mVisualsPipeline;
        delete mShadowPipeline;

        delete mHdrRenderer;
        delete mBlurRenderer;
        delete mBloomRenderer;

        mTerrainBuilder.free();

        delete mEntityControl;

#ifdef IMGUI
        ImguiCore::free();
#endif

        delete mEnvironment;
        delete mScene;

        delete mDevice;
        delete mWindow;

#ifdef DEBUG
        delete mDebugger;
        Logger::free();
#endif

    }

    void Application::onRender(const float dt) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glStencilMask(GL_FALSE);
        mShadowPipeline->render();
        mPbrPipeline->render();
        mRayTraceRenderer->render();
        mColorFrame = mPbrPipeline->getColorFrame();
        mDepthFrame = mPbrPipeline->getDepthFrame();

        // Post processing chain
        glDisable(GL_CULL_FACE);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        mScreenRenderer->getParams().buffer = mPbrPipeline->getRenderTarget();
        onRenderPostFX(dt);

        // UI pipeline
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        mUiPipeline->blitColorDepth(mWidth, mHeight, mColorFrame.id, mDepthFrame.id);
        mUiPipeline->render();
        mColorFrame = mUiPipeline->getColorFrame();
        mDepthFrame = mUiPipeline->getDepthFrame();
        mFinalRenderTarget = mUiPipeline->getRenderTarget();

        glDisable(GL_BLEND);

#ifdef IMGUI

        glEnable(GL_CULL_FACE);

        mVisualsPipeline->blitColorDepth(mWidth, mHeight, mColorFrame.id, mDepthFrame.id);
        mVisualsPipeline->render();
        mColorFrame = mVisualsPipeline->getColorFrame();
        mDepthFrame = mVisualsPipeline->getDepthFrame();
        mFinalRenderTarget = mVisualsPipeline->getRenderTarget();

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

#ifdef DEBUG

        if (mDebugRenderTarget.id == InvalidImageBuffer) {
            mScreenRenderer->getParams().buffer = mFinalRenderTarget;
        } else {
            mScreenRenderer->getParams().buffer = mDebugRenderTarget;
        }

#else

        mScreenRenderer->getParams().buffer = mFinalRenderTarget;

#endif

        mScreenRenderer->render();
        renderImgui(dt);

#else

        glDisable(GL_DEPTH_TEST);

#ifdef DEBUG

        if (mDebugRenderTarget.id == InvalidImageBuffer) {
            mScreenRenderer->getParams().buffer = mFinalRenderTarget;
        } else {
            mScreenRenderer->getParams().buffer = mDebugRenderTarget;
        }

#else

        mScreenRenderer->getParams().buffer = mFinalRenderTarget;

#endif

        mScreenRenderer->renderBackBuffer();

#endif

    }

    void Application::onRenderPostFX(const float dt) {
        // Bloom effect
        if (mBloomRenderer->isEnabled) {
            mBloomRenderer->getHdrBuffer() = mScreenRenderer->getParams().buffer;
            mBloomRenderer->render();
            mScreenRenderer->getParams().buffer = mBloomRenderer->getRenderTarget();
            mColorFrame = mBloomRenderer->getColorFrame();
        }
        // HDR effect
        if (mHdrRenderer->isEnabled) {
            mHdrRenderer->getParams().sceneBuffer = mScreenRenderer->getParams().buffer;
            mHdrRenderer->render();
            mScreenRenderer->getParams().buffer = mHdrRenderer->getRenderTarget();
            mColorFrame = mHdrRenderer->getColorFrame();
        }
        // FXAA effect
        if (mFxaaRenderer->isEnabled) {
            mFxaaRenderer->getParams().srcBuffer = mScreenRenderer->getParams().buffer;
            mFxaaRenderer->render();
            mScreenRenderer->getParams().buffer = mFxaaRenderer->getRenderTarget();
            mColorFrame = mFxaaRenderer->getColorFrame();
        }
        // Blur effect
        if (mBlurRenderer->isEnabled) {
            mBlurRenderer->getParams().sceneBuffer = mScreenRenderer->getParams().buffer;
            mBlurRenderer->render();
            mScreenRenderer->getParams().buffer = mBlurRenderer->getRenderTarget();
            mColorFrame = mBlurRenderer->getColorFrame();
        }
    }

    void Application::renderImgui(const float dt) {
        ImguiCore::begin();
        onRenderImgui(dt);
        ImguiCore::end();

        if (ImguiCore::close) {
            mWindow->close();
        }
    }

    void Application::onRenderImgui(const float dt) {
    }

    void Application::onSimulate(const float dt) {
    }

    void Application::onUpdateInput(const float dt) {
    }

    void Application::loadHdrShinyImage(const char* filepath) {
        Image shinyImage = ImageReader::read(filepath);
        ImageBuffer shinyBuffer;
        shinyBuffer.init();
        shinyBuffer.load(shinyImage);
        shinyImage.free();
        mHdrRenderer->getParams().shinyBuffer = shinyBuffer;
    }

    void Application::setScene(Scene* scene) {
        mScene = scene;

        mShadowPipeline->scene = scene;

        mPbrPipeline->scene = scene;

        mUiPipeline->scene = scene;

        mVisualsPipeline->scene = scene;

#ifdef IMGUI
        ImguiCore::scene = scene;
#endif

        LightStorage::scene = scene;

    }

    void Application::setEnvironment(Environment* environment) {
        mEnvironment = environment;
        mEnvRenderer->setEnvironment(environment);
        mPbrPipeline->setEnvironment(environment);
    }

    void Application::setCamera(Camera* camera) {
        mCamera = camera;
        mShadowPipeline->directShadow.camera = camera;
        mShadowPipeline->pointShadow.camera = camera;

#ifdef IMGUI
        ImguiCore::camera = mCamera;
#endif
    }

    void Application::initLogger() {
        Logger::init(mTitle, 32);
    }

    void Application::initWindow() {
        Window::setWindowErrorCallback(onWindowError);
        mWindow = new Window(
                0, 0,
                mWidth, mHeight,
                mTitle
        );
        mWindow->setTheme(mThemeMode);
        mWindow->initCallbacks<Application>(this);
        mWindow->setWindowCloseCallback<Application>();
        mWindow->setWindowResizeCallback<Application>();
        mWindow->setWindowMoveCallback<Application>();
        mWindow->setFramebufferResizeCallback<Application>();
        mWindow->setKeyCallback<Application>();
        mWindow->setMouseCallback<Application>();
        mWindow->setMouseCursorCallback<Application>();
        mWindow->setMouseScrollCallback<Application>();

        mWindow->loadIcon(mLogoName);
    }

    void Application::initApi() {
        mDevice = new Device(mWidth, mHeight);

#ifdef DEBUG
        mDebugger = new Debugger();
#endif

        mScreenRenderer = new ScreenRenderer(mWidth, mHeight);

        mHdrRenderer = new HdrRenderer(mWidth, mHeight);
        mHdrRenderer->isEnabled = true;
        mHdrRenderer->getParams().exposure.value = 1.2f;
        mHdrRenderer->updateExposure();

        mBlurRenderer = new BlurRenderer(mWidth, mHeight);
        mBlurRenderer->isEnabled = false;

        mBloomRenderer = new BloomRenderer(mWidth, mHeight);
        mBloomRenderer->isEnabled = true;

        mSsaoRenderer = new SsaoRenderer(mWidth, mHeight);
        mSsaoRenderer->isEnabled = true;

        mFxaaRenderer = new FXAARenderer(mWidth, mHeight);
        mFxaaRenderer->isEnabled = false;

        mTransparentRenderer = new TransparentRenderer(mWidth, mHeight);
        mTransparentRenderer->isEnabled = true;

        mShadowPipeline = new ShadowPipeline(mScene, mWidth, mHeight, mCamera);
        mShadowPipeline->directShadow.filterSize = 9;

        mEnvRenderer = new EnvRenderer(mWidth, mHeight);

        mOutlineRenderer = new OutlineRenderer();

        mPbrPipeline = new PBR_Pipeline(
                mScene, mWidth, mHeight,
                mSsaoRenderer,
                mTransparentRenderer,
                mEnvRenderer,
                mOutlineRenderer
        );
        mPbrPipeline->setDirectShadow(&mShadowPipeline->directShadow);
        mPbrPipeline->setPointShadow(&mShadowPipeline->pointShadow);
        mPbrPipeline->terrain = &mTerrainBuilder.terrain;

        mUiPipeline = new UI_Pipeline(mScene, mWidth, mHeight);
        mVisualsPipeline = new VisualsPipeline(mScene, mWidth, mHeight);

        mRayTraceRenderer = new RayTraceRenderer(mWidth, mHeight);

        LightStorage::scene = mScene;
        LightStorage::init();

        FontAtlas::init();

        EnvStorage::init();
    }

    void Application::initNetwork() {
        NetworkCore::init();
        mTcpClient = new TCPClient();
//        mDatabaseService = new DatabaseService(mTcpClient);
    }

    void Application::initImgui() {
        ImguiCore::init(mWindow, "#version 460 core");
        ImguiCore::setIniFilename(mTitle);
        ImguiCore::addRegularFont("Assets/fonts/Roboto-Regular.ttf", 16.0f);
        ImguiCore::addBoldFont("Assets/fonts/Roboto-Bold.ttf", 16.0f);
        ImguiCore::setFont(ImguiCore::boldFont);
        ImguiCore::addIconFont("Assets/fonts/codicon.ttf", 20.0f);
        ImguiCore::setDarkTheme();
        ImguiCore::loadLogo(mLogoName);

        ImguiCore::callback = this;

        ImguiCore::shadowPipeline = mShadowPipeline;
        ImguiCore::pbrPipeline = mPbrPipeline;
        ImguiCore::uiPipeline = mUiPipeline;
        ImguiCore::visualsPipeline = mVisualsPipeline;

        ImguiCore::screenRenderer = mScreenRenderer;
        ImguiCore::hdrRenderer = mHdrRenderer;
        ImguiCore::blurRenderer = mBlurRenderer;
        ImguiCore::bloomRenderer = mBloomRenderer;
        ImguiCore::ssaoRenderer = mSsaoRenderer;
        ImguiCore::fxaaRenderer = mFxaaRenderer;

        ImguiCore::transparentRenderer = mTransparentRenderer;

        onCreateImgui();
    }

    void Application::printDt() {
        mPrintLimiter++;
        if (mPrintLimiter > 100) {
            mPrintLimiter = 0;
            printFPS(Timer::getDeltaMillis());
        }
    }

    void Application::onWindowClose() {
        trace("");
    }

    void Application::onWindowResize(int w, int h) {
        info("width={0}, height={1}", w, h);
        mWindow->onResized(w, h);
    }

    void Application::onWindowMove(int x, int y) {
        info("x={0}, y={1}", x, y);
        mWindow->onMoved(x, y);
    }

    void Application::onFramebufferResized(int w, int h) {
        info("width={0}, height={1}", w, h);

        mWidth = w;
        mHeight = h;

        if (mCamera) {
            mCamera->resize();
        }

        mWindow->onFrameResized(w, h);

        mScreenRenderer->resize(w, h);
        mHdrRenderer->resize(w, h);
        mBlurRenderer->resize(w, h);
        mBloomRenderer->resize(w, h);
        mSsaoRenderer->resize(w, h);
        mFxaaRenderer->resize(w, h);

        mTransparentRenderer->resize(w, h);

        mShadowPipeline->resize(w, h);
        mPbrPipeline->resize(w, h);
        mUiPipeline->resize(w, h);
        mVisualsPipeline->resize(w, h);

        mRayTraceRenderer->resize(w, h);

#ifdef IMGUI
        ImguiCore::frameBufferResized = true;
#endif
    }

    void Application::onKeyPress(const KEY key) {
        trace("");

        switch (key) {

            case Esc:
                mWindow->close();
                break;

            case F:
                mWindow->toggleWindowMode();
                break;

#ifdef DEBUG

            case D1:
                mDebugRenderTarget.id = InvalidImageBuffer;
                break;

            case D2:
                mDebugRenderTarget = mPbrPipeline->getGBuffer().position;
                break;

            case D3:
                mDebugRenderTarget = mPbrPipeline->getGBuffer().normal;
                break;

            case D4:
                mDebugRenderTarget = mPbrPipeline->getGBuffer().albedo;
                break;

            case D5:
                mDebugRenderTarget = mPbrPipeline->getGBuffer().pbrParams;
                break;

            case D6:
                mDebugRenderTarget = mPbrPipeline->getGBuffer().emission;
                break;

            case D7:
                mDebugRenderTarget = mShadowPipeline->directShadow.map.buffer;
                break;

            case D8:
                mDebugRenderTarget = mSsaoRenderer->getRenderTarget();
                break;

            case D9:
                mDebugRenderTarget = mTransparentRenderer->getRenderTarget();
                break;

            case D0:
                mDebugRenderTarget = mUiPipeline->getRenderTarget();
                break;

            case P:
                mDebugRenderTarget = mVisualsPipeline->getRenderTarget();
                break;

            case C:
                mDebugRenderTarget = mRayTraceRenderer->getRenderTarget();
                break;

#endif

            default:
                break;

        }
    }

    void Application::onKeyRelease(const KEY key) {
        trace("");
    }

    void Application::onMousePress(const int button) {
        trace("");
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            Cursor mouse_cursor = mWindow->mouseCursor();
            static PBR_Pixel pixel;
            mPbrPipeline->readPixel(pixel, (int) mouse_cursor.x, (int) mouse_cursor.y);
//            info("Read pixel from [x,y] = [" << mouse_cursor.x << "," << mouse_cursor.y << "]")
            mEntityControl->select(mouse_cursor.x, mouse_cursor.y);
        }
    }

    void Application::onMouseRelease(const int button) {
        trace("");
    }

    void Application::onMouseCursor(const double x, const double y) {
    }

    void Application::onMouseScroll(const double x, const double y) {
    }

    void Application::resize(const int w, const int h) {
        onFramebufferResized(w, h);
    }

    void Application::resample(const int samples) {
    }

    void Application::onEntitySelected(Entity entity, const double x, const double y) {
        info("onEntitySelected: [{0}, {1}]", x, y);
        auto& selected = entity.getComponent<Selectable>()->enable;
        selected = !selected;
//        if (selected) {
//            entity.addComponent<PolygonVisual>();
//            entity.addComponent<NormalVisual>();
//        } else {
//            entity.removeComponent<PolygonVisual>();
//            entity.removeComponent<NormalVisual>();
//        }
    }

    void Application::onEntityDragged(Entity entity, double x, double y) {
        info("onEntityDragged: [{0}, {1}]", x, y);
    }

    void Application::onEntityHovered(Entity entity, double x, double y) {
        info("onEntityHovered: [{0}, {1}]", x, y);
        entity.getComponent<Material>()->color = { 5, 5, 5, 1 };
    }

}