#include <core/application.h>

namespace gl {

    static void onWindowError(int code, const char* message) {
        error("Error code: {0}\nError message: {1}", code, message);
    }

    Application::Application(const char* title, int width, int height, const char* logoName, const ThemeMode themeMode)
    : mTitle(title), mWidth(width), mHeight(height), mLogoName(logoName), mThemeMode(themeMode) {
#ifdef DEBUG
        initLogger();
#endif
        initWindow();
        initApi();
        initCamera();
        initLight();
        initEnvironment();
        initScene();
        initText();
#ifdef IMGUI
        initImgui();
#endif
    }

    Application::~Application() {
        mEnvironment.free();

        delete mTransparentRenderer;

        delete mRayTraceRenderer;

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

        mWoodSphere.free();
        mWoodSphere.material()->free();
        mWoodSphere.drawable()->free();

        mMetalSphere.free();
        mMetalSphere.material()->free();
        mMetalSphere.drawable()->free();

        mRockSphere.free();
        mRockSphere.material()->free();
        mRockSphere.drawable()->free();

        mPointLightVisual.drawable.free();

        mBackpack.free();
        mBackpack.material()->free();
        mBackpack.drawable()->free();
        mBackpackModel.free();

        mHuman.free();
        mHuman.material()->free();
        mHuman.drawable()->free();
        mHumanModel.free();

        delete mEntityControl;

        delete mCamera;

#ifdef IMGUI
        ImguiCore::free();
#endif

        delete mDevice;
        delete mWindow;

#ifdef DEBUG
        delete mDebugger;
        Logger::free();
#endif
    }

    void Application::run() {
        while (mRunning) {
            Timer::begin();

            mRunning = mWindow->isOpen();

            mWindow->poll();

            simulate();

            render();

            mWindow->swap();

            Timer::end();

            printDt();
        }
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

        mDevice = new Device(mWidth, mHeight);

#ifdef DEBUG
        mDebugger = new Debugger();
#endif
    }

    void Application::initImgui() {
        ImguiCore::init(mWindow, "#version 460 core");
        ImguiCore::setIniFilename(mTitle);
        ImguiCore::addRegularFont("fonts/Roboto-Regular.ttf", 16.0f);
        ImguiCore::addBoldFont("fonts/Roboto-Bold.ttf", 16.0f);
        ImguiCore::setFont(ImguiCore::boldFont);
        ImguiCore::addIconFont("fonts/codicon.ttf", 20.0f);
        ImguiCore::setDarkTheme();
        ImguiCore::loadLogo(mLogoName);

        ImguiCore::callback = this;
        ImguiCore::camera = mCamera;
        ImguiCore::scene = &mScene;

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
    }

    void Application::initScene() {
        mBackpack = {
                &mScene,
                { -3, 1.5, 5 },
                { -90, 0, 0 },
                { 1, 1, 1 }
        };

        mTerrainBuilder = {
                &mScene,
                { 0, 0.1, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 },
                32
        };

        mHuman = {
                &mScene,
                { 0, 0, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mRockSphere = {
                &mScene,
                { 0, 1.5, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mWoodSphere = {
                &mScene,
                { 0, 1.5, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mMetalSphere = {
                &mScene,
                { 0, 1.5, 8 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mEntityControl = new EntityControl(&mScene, mCamera);

        mTerrainBuilder.addComponent<Selectable>();
        mTerrainBuilder.addComponent<Draggable>();

        mRockSphere.addComponent<Opaque>();
        mRockSphere.addComponent<Selectable>(onEntitySelected);
        mRockSphere.addComponent<Draggable>(onEntityDragged);
        mRockSphere.addComponent<SphereCollider>(mRockSphere.transform()->translation, 3.0f);
        mRockSphere.addComponent<Shadowable>();

        mWoodSphere.addComponent<Transparent>();
        mWoodSphere.addComponent<Selectable>(onEntitySelected);
        mWoodSphere.addComponent<Draggable>(onEntityDragged);
        mWoodSphere.addComponent<Shadowable>();

        mMetalSphere.addComponent<Opaque>();
        mMetalSphere.addComponent<Selectable>(onEntitySelected);
        mMetalSphere.addComponent<Draggable>(onEntityDragged);
        mMetalSphere.addComponent<Shadowable>();

        mBackpack.addComponent<Opaque>();
        mBackpack.addComponent<Selectable>(onEntitySelected);
        mBackpack.addComponent<Draggable>(onEntityDragged);
        mBackpack.addComponent<Shadowable>();

        mHuman.addComponent<Opaque>();
        mHuman.addComponent<Selectable>(onEntitySelected);
        mHuman.addComponent<Draggable>(onEntityDragged);

        // setup 3D model
        mBackpackModel.generate("models/backpack/backpack.obj");
        mBackpackModel.init(*mBackpack.getComponent<DrawableElements>());
//        model_shadow.init(model);
        mBackpack.material()->load(
                true,
                "models/backpack/diffuse.jpg",
                "models/backpack/normal.png",
                null,
                "models/backpack/specular.jpg",
                "models/backpack/roughness.jpg",
                "models/backpack/ao.jpg"
        );
        mBackpack.material()->metallicFactor = 1;
        mBackpack.material()->roughnessFactor = 0.5;
        mBackpack.material()->aoFactor = 1;

        // setup mHuman model
        mHumanModel.generate("models/dancing-stormtrooper/source/silly_dancing.fbx");
        mHumanModel.init(*mHuman.getComponent<DrawableElements>());
        mHuman.material()->load(
                false,
                "models/dancing-stormtrooper/textures/Stormtrooper_D.png",
                null,
                null,
                null,
                null,
                null
        );
        mHuman.material()->metallicFactor = 0.5;
        mHuman.material()->roughnessFactor = 0.5;
        mHuman.material()->aoFactor = 1.0;
        mHumanAnimator = Animator(&mHumanModel.animation);

        // setup horizontal plane
        mTerrainBuilder.terrain.material.load(
                false,
                null,
                "images/bumpy-rockface1-bl/normal.png",
                null,
                "images/bumpy-rockface1-bl/metallic.png",
                "images/bumpy-rockface1-bl/roughness.png",
                "images/bumpy-rockface1-bl/ao.png"
        );
        mTerrainBuilder.terrain.material.color = { 1, 1, 1, 1 };
        mTerrainBuilder.terrain.material.metallicFactor = 1;
        mTerrainBuilder.terrain.material.roughnessFactor = 1;
        mTerrainBuilder.terrain.material.aoFactor = 1;

        // setup sphere
        SphereTBN sphere_geometry = { 64, 64 };
        sphere_geometry.init(*mWoodSphere.getComponent<DrawableElements>());
        sphere_geometry.init(*mMetalSphere.getComponent<DrawableElements>());
//        SphereTBN sphere_shadow_geometry;
//        sphere_shadow_geometry.init_default();
        // setup rock sphere
        mRockSphereGeometry.init(*mRockSphere.getComponent<DrawableElements>());
//        sphere_rock_shadow_geometry.x_segments = 2047;
//        sphere_rock_shadow_geometry.y_segments = 2047;
//        sphere_rock_shadow_geometry.init_default(sphere_rock_shadow);

        {
            mRockSphere.material()->color = {1, 1, 1, 1 };
            mRockSphere.material()->load(
                    false,
                    "images/columned_lava/albedo.png",
                    "images/columned_lava/normal.png",
                    null,
                    "images/columned_lava/metallic.png",
                    "images/columned_lava/roughness.png",
                    "images/columned_lava/ao.png",
                    "images/columned_lava/emission.png"
            );
            mRockSphere.material()->metallicFactor = 1;
            mRockSphere.material()->roughnessFactor = 1;
            mRockSphere.material()->aoFactor = 1;
            mRockSphere.material()->emissionFactor = { 1, 1, 1 };

            // displacement
            {
                // displace rock sphere
                Image rock_height_map = ImageReader::read("images/columned_lava/height.png");
                rock_height_map.resize(mRockSphereGeometry.xSegments + 1, mRockSphereGeometry.ySegments + 1);

                mRockSphere.addComponent<DisplacementTBN>();
                auto* rock_sphere_displacement = mRockSphere.getComponent<DisplacementTBN>();
                rock_sphere_displacement->setOriginVertices(&mRockSphereGeometry.vertices);
                rock_sphere_displacement->scale = 0.1f;
                rock_sphere_displacement->map = HeightMap(rock_height_map);
                rock_sphere_displacement->displace(*mRockSphere.getComponent<DrawableElements>());

                rock_height_map.free();

                // displace terrain
                Image terrain_height_map = ImageReader::read("images/terrain/earth_heightmap.png");
                terrain_height_map.resize(mTerrainBuilder.size(), mTerrainBuilder.size());

                mTerrainBuilder.displaceWith(MidPointFormation(mTerrainBuilder.size(), mTerrainBuilder.size(), 10, 0, 1), 1);
                mImageMixer.displacementMap = &mTerrainBuilder.displacement.map;
                mImageMixer.addImage({-1.0, -0.5, 0 }, "images/terrain/rock_tile.png");
                mImageMixer.addImage({0, 0.1, 0.2 }, "images/terrain/sand_tile.jpg");
                mImageMixer.addImage({0.20, 0.45, 0.70 }, "images/terrain/grass_tile.png");
                mImageMixer.addImage({0.70, 0.77, 0.85 }, "images/terrain/rock_tile.png");
                mImageMixer.addImage({0.85, 0.90, 1.0 }, "images/terrain/snow_tile.png");
                mImageMixer.mix(mTerrainBuilder.size(), mTerrainBuilder.size());
                ImageWriter::write("images/terrain/mixed_image.png", mImageMixer.mixedImage);

                terrain_height_map.free();

                ImageParams params;
                params.minFilter = GL_LINEAR_MIPMAP_LINEAR;
                mTerrainBuilder.terrain.material.albedo.init();
                mTerrainBuilder.terrain.material.albedo.load(mImageMixer.mixedImage, params);
                mTerrainBuilder.terrain.material.enableAlbedo = mTerrainBuilder.terrain.material.albedo.id != InvalidImageBuffer;
            }

            mWoodSphere.material()->load(
                    false,
                    null,
                    "images/cheap-plywood1-bl/normal.png",
                    null,
                    "images/cheap-plywood1-bl/metallic.png",
                    "images/cheap-plywood1-bl/roughness.png",
                    "images/cheap-plywood1-bl/ao.png"
            );
            mWoodSphere.material()->color = { 1, 1, 1, 0.5 };
            mWoodSphere.material()->metallicFactor = 1;
            mWoodSphere.material()->roughnessFactor = 1;
            mWoodSphere.material()->aoFactor = 1;

            mMetalSphere.material()->load(
                    false,
                    "images/light-gold-bl/albedo.png",
                    "images/light-gold-bl/normal.png",
                    null,
                    "images/light-gold-bl/metallic.png",
                    "images/light-gold-bl/roughness.png",
                    null
            );
            mMetalSphere.material()->metallicFactor = 1;
            mMetalSphere.material()->roughnessFactor = 1;
            mMetalSphere.material()->aoFactor = 1;
        }

        mRay.direction = { 0, 0, 0, 0 };
        mSphere.center = { 0, 0, 0 };
        mSphere.radius = 0.0f;
        mRayTraceRenderer->update(mRay);
        mRayTraceRenderer->update(mSphere);
    }

    void Application::initApi() {
        mScreenRenderer = new ScreenRenderer(mWidth, mHeight);

        mHdrRenderer = new HdrRenderer(mWidth, mHeight);
        mHdrRenderer->isEnabled = true;
        Image shinyImage = ImageReader::read("images/lens_dirt/lens_dirt.png");
        ImageBuffer shinyBuffer;
        shinyBuffer.init();
        shinyBuffer.load(shinyImage);
        shinyImage.free();
        mHdrRenderer->getParams().shinyBuffer = shinyBuffer;
        mHdrRenderer->getParams().exposure.value = 1.2f;
        mHdrRenderer->updateExposure();

        mBlurRenderer = new BlurRenderer(mWidth, mHeight);
        mBlurRenderer->isEnabled = false;

        mBloomRenderer = new BloomRenderer(mWidth, mHeight);
        mBloomRenderer->isEnabled = true;

        mSsaoRenderer = new SsaoRenderer(mWidth, mHeight);
        mSsaoRenderer->isEnabled = true;

        mFxaaRenderer = new FXAARenderer(mWidth, mHeight);
        mFxaaRenderer->isEnabled = true;

        mTransparentRenderer = new TransparentRenderer(mWidth, mHeight);
        mTransparentRenderer->isEnabled = true;

        mShadowPipeline = new ShadowPipeline(&mScene, mWidth, mHeight, mCamera);
        mShadowPipeline->directShadow.filterSize = 9;

        mPbrPipeline = new PBR_Pipeline(&mScene, mWidth, mHeight, mSsaoRenderer, mTransparentRenderer);
        mPbrPipeline->setDirectShadow(&mShadowPipeline->directShadow);
        mPbrPipeline->setPointShadow(&mShadowPipeline->pointShadow);
        mPbrPipeline->terrain = &mTerrainBuilder.terrain;

        mUiPipeline = new UI_Pipeline(&mScene, mWidth, mHeight);
        mVisualsPipeline = new VisualsPipeline(&mScene, mWidth, mHeight);

        mRayTraceRenderer = new RayTraceRenderer(mWidth, mHeight);
    }

    void Application::initEnvironment() {
        mEnvironment.enable = true;
        mEnvironment.resolution = { 1024, 1024 };
        mEnvironment.prefilterResolution = {512, 512 };
        mEnvironment.hdr.loadHDR("images/hdr/Arches_E_PineTree_3k.hdr", true);
        mEnvironment.init();
        mPbrPipeline->setEnvironment(&mEnvironment);
        mPbrPipeline->generateEnv();
    }

    void Application::initCamera() {
        mCamera = new Camera(0, mWindow);
        mCamera->zFar = 1000.0f;
        mCamera->position = {-5, 2, 10 };
    }

    void Application::initLight() {
        LightStorage::scene = &mScene;
        LightStorage::init();
        // setup light presentation
        CubeDefault().init(mPointLightVisual.drawable);
        // setup mSunlight
        mSunlight = &mScene;
        mSunlight.value().color = { 244, 233, 155, 0.1f };
        mSunlight.value().position = { 0, 10, 0, 0 };
        mSunlight.value().direction = { 0, 0, 0 };
        // setup point lights
        for (auto& pointLight : mPointLights) {
            pointLight = &mScene;
        }
        mPointLights[0].value().position = { 0, 0, 0, 1.0f };
        mPointLights[0].value().color = { 0, 0, 0, 1.0f };
        mPointLights[1].value().position = {4, 3, 0, 1 };
        mPointLights[1].value().color = {0, 1, 0, 0 };
        mPointLights[2].value().position = {-4, 4, 8, 1 };
        mPointLights[2].value().color = {0, 0, 1, 0 };
        mPointLights[3].value().position = {4, 5, 8, 1 };
        mPointLights[3].value().color = {1, 0, 1, 0 };
        // setup mFlashlight
        mFlashlight = &mScene;
        mFlashlight.value().position = { mCamera->position, 0 };
        mFlashlight.value().direction = { mCamera->front, 0 };
        mFlashlight.value().color = {0, 0, 0, 0 };
    }

    void Application::simulate() {
        mCamera->move(Timer::getDeltaMillis());
        // bind flashlight to camera
        mFlashlight.value().position = { mCamera->position, 0 };
        mFlashlight.value().direction = { mCamera->front, 0 };

        // rotate object each frame
        float f = 0.05f;
        mRockSphere.transform()->rotation.y += f;
        mWoodSphere.transform()->rotation.y += f * 2;
        mMetalSphere.transform()->rotation.y += f * 4;
        mHuman.transform()->rotation.y += f * 4;

        // skeletal animations
        {
            // animate human
//            mHumanAnimator.update(dt / 1000.0f);
//            pbr.update_bones(mHumanAnimator.bone_transforms);
        }

        // bind first point light to rock sphere position and emission color
        mPointLights[0].value().position = { mRockSphere.transform()->translation, 1.0 };

        LightStorage::update();
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

        mWindow->onFrameResized(w, h);

        mCamera->resize();

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

        if (key == KEY::Esc)
            mWindow->close();

        else if (key == KEY::F)
            mWindow->toggleWindowMode();
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
        mCamera->onMouseCursor(x, y, Timer::getDeltaMillis());
        mEntityControl->drag(x, y);
    }

    void Application::onMouseScroll(const double x, const double y) {
        mCamera->onMouseScroll(y, Timer::getDeltaMillis());
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

    void Application::renderPostFX() {
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

    void Application::renderDebugScreen() {
        if (mWindow->isKeyPress(KEY::D1)) {
            mScreenRenderer->getParams().buffer = mPbrPipeline->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::D2)) {
            mScreenRenderer->getParams().buffer = mPbrPipeline->getGBuffer().position;
        }

        else if (mWindow->isKeyPress(KEY::D3)) {
            mScreenRenderer->getParams().buffer = mPbrPipeline->getGBuffer().normal;
        }

        else if (mWindow->isKeyPress(KEY::D4)) {
            mScreenRenderer->getParams().buffer = mPbrPipeline->getGBuffer().albedo;
        }

        else if (mWindow->isKeyPress(KEY::D5)) {
            mScreenRenderer->getParams().buffer = mPbrPipeline->getGBuffer().pbrParams;
        }

        else if (mWindow->isKeyPress(KEY::D6)) {
            mScreenRenderer->getParams().buffer = mPbrPipeline->getGBuffer().emission;
        }

        else if (mWindow->isKeyPress(KEY::D7)) {
            mScreenRenderer->getParams().buffer = mShadowPipeline->directShadow.map.buffer;
        }

        else if (mWindow->isKeyPress(KEY::D8)) {
            mScreenRenderer->getParams().buffer = mSsaoRenderer->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::D9)) {
            mScreenRenderer->getParams().buffer = mTransparentRenderer->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::D0)) {
            mScreenRenderer->getParams().buffer = mUiPipeline->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::P)) {
            mScreenRenderer->getParams().buffer = mVisualsPipeline->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::T)) {
            mScreenRenderer->getParams().buffer = mFontRobotoRegular->buffer;
        }

        else if (mWindow->isKeyPress(KEY::C)) {
            mScreenRenderer->getParams().buffer = mRayTraceRenderer->getRenderTarget();
        }
    }

    void Application::render() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glStencilMask(GL_FALSE);

        mShadowPipeline->render();

        mPbrPipeline->render();
        mColorFrame = mPbrPipeline->getColorFrame();
        mDepthFrame = mPbrPipeline->getDepthFrame();

        // todo move into ray trace pipeline
        mRayTraceRenderer->render();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        mScreenRenderer->getParams().buffer = mPbrPipeline->getRenderTarget();
        renderPostFX();

#ifdef DEBUG
        renderDebugScreen();
#endif

#ifdef IMGUI

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        mUiPipeline->blitColorDepth(mWidth, mHeight, mColorFrame.id, mDepthFrame.id);
        mUiPipeline->render();
        mColorFrame = mUiPipeline->getColorFrame();
        mDepthFrame = mUiPipeline->getDepthFrame();

        glEnable(GL_CULL_FACE);

        mVisualsPipeline->blitColorDepth(mWidth, mHeight, mColorFrame.id, mDepthFrame.id);
        mVisualsPipeline->render();
        mColorFrame = mVisualsPipeline->getColorFrame();
        mDepthFrame = mVisualsPipeline->getDepthFrame();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        mScreenRenderer->getParams().buffer = mVisualsPipeline->getRenderTarget();
        mScreenRenderer->render();
        renderImgui();
#else
        mScreenRenderer->renderBackBuffer();
#endif

    }

    void Application::initText() {
        FontAtlas::init();

        mFontRobotoRegular = FontAtlas::load("fonts/Roboto-Regular.ttf", 40);
        mFontRobotoRegular->saveBmp("fonts/Roboto-Regular.bmp");
        mFontRobotoRegular->saveWidths("fonts/Roboto-Regular.widths");

        mTextLabel = &mScene;
        mTextLabel.addComponent<Text3d>(mFontRobotoRegular, "Hello World!");
        mTextLabel.getComponent<Text3d>()->transform.translation = { 0, 5, 0 };
    }

    void Application::renderImgui() {
        ImguiCore::begin();

        Toolbar::render();
        ScreenWindow::render();
        PropertiesWindow::render();
        EntityWindow::render();
        ComponentWindow::render();

        ImguiCore::end();

        if (ImguiCore::close) {
            mWindow->close();
        }
    }
}