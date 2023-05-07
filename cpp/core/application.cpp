#include <core/application.h>

namespace gl {

    static void onWindowError(int code, const char* message) {
        error("Error code: " << code << "\nError message: " << message);
    }

    Application::Application(const char* title, int width, int height)
    : mTitle(title), mWidth(width), mHeight(height) {
#ifdef DEBUG
        initLogger();
#endif
        initWindow();
        initApi();
        initCamera();
        initLight();
        initScene();
        initText();
#ifdef IMGUI
        initImgui();
#endif
    }

    Application::~Application() {
        free();
    }

    void Application::run() {
        while (mRunning) {
            mBeginTime = glfwGetTime();
            mRunning = mWindow->isOpen();

            mWindow->poll();

            simulate();

            render();

            mWindow->swap();

            mDeltaTime = ((float)glfwGetTime() - mBeginTime) * 1000;

            printDt();
        }
    }

    void Application::initLogger() {
        Logger::init(mTitle);
    }

    void Application::initWindow() {
        Window::setWindowErrorCallback(onWindowError);
        mWindow = new Window(
                0, 0,
                mWidth, mHeight,
                mTitle,
                WindowFlags::Sync,
                4, 6, GLFW_OPENGL_CORE_PROFILE
        );
        mWindow->initCallbacks<Application>(this);
        mWindow->setWindowCloseCallback<Application>();
        mWindow->setWindowResizeCallback<Application>();
        mWindow->setWindowMoveCallback<Application>();
        mWindow->setFramebufferResizeCallback<Application>();
        mWindow->setKeyCallback<Application>();
        mWindow->setMouseCallback<Application>();
        mWindow->setMouseCursorCallback<Application>();
        mWindow->setMouseScrollCallback<Application>();

        mDevice = new Device(mWidth, mHeight);

#ifdef DEBUG
        mDebugger = new Debugger();
#endif
    }

    void Application::initImgui() {
        mImgui = new ImguiCore(mWindow, "#version 460 core");
        mImgui->setIniFilename(mTitle);
        mImgui->addFont("fonts/Roboto-Regular.ttf", 14.0f);

        ScreenProperties::title = "Screen Properties";
        ScreenProperties::resolution = { 256, 256 };
        ScreenProperties::position = { 0, 0 };
        ScreenProperties::screenRenderer = mScreenRenderer;
        ScreenProperties::hdrRenderer = mHdrRenderer;
        ScreenProperties::blurRenderer = mBlurRenderer;
        ScreenProperties::bloomRenderer = mBloomRenderer;
        ScreenProperties::ssaoRenderer = mSsaoRenderer;
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
                { 0, 0.25, 4 },
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

        mEntityControl = new EntityControl(&mScene, &mCamera);

        mTerrainBuilder.addComponent<Selectable>();
        mTerrainBuilder.addComponent<Draggable>();

        mRockSphere.addComponent<PBR_Component_DeferredCull>();
        mRockSphere.addComponent<Selectable>(onEntitySelected);
        mRockSphere.addComponent<Draggable>(onEntityDragged);
        mRockSphere.addComponent<SphereCollider>(mRockSphere.transform()->translation, 3.0f);
        mRockSphere.addComponent<Shadowable>();

        mWoodSphere.addComponent<PBR_Component_DeferredCull>();
        mWoodSphere.addComponent<Selectable>(onEntitySelected);
        mWoodSphere.addComponent<Draggable>(onEntityDragged);
        mWoodSphere.addComponent<Shadowable>();

        mMetalSphere.addComponent<PBR_Component_DeferredCull>();
        mMetalSphere.addComponent<Selectable>(onEntitySelected);
        mMetalSphere.addComponent<Draggable>(onEntityDragged);
        mMetalSphere.addComponent<Shadowable>();

        mBackpack.addComponent<PBR_Component_DeferredCull>();
        mBackpack.addComponent<Selectable>(onEntitySelected);
        mBackpack.addComponent<Draggable>(onEntityDragged);
        mBackpack.addComponent<Shadowable>();

        mHuman.addComponent<PBR_SkeletalComponent_DeferredCull>();
        mHuman.addComponent<Selectable>(onEntitySelected);
        mHuman.addComponent<Draggable>(onEntityDragged);

        // setup 3D model
        mBackpackModel.generate("models/backpack/backpack.obj");
        mBackpackModel.init(*mBackpack.getComponent<DrawableElements>());
//        model_shadow.init(model);
        mBackpack.material()->init(
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
        mHuman.material()->init(
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
        mTerrainBuilder.terrain.material.init(
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
            mRockSphere.material()->init(
                    false,
                    "images/bumpy-rockface1-bl/albedo.png",
                    "images/bumpy-rockface1-bl/normal.png",
                    null,
                    "images/bumpy-rockface1-bl/metallic.png",
                    "images/bumpy-rockface1-bl/roughness.png",
                    "images/bumpy-rockface1-bl/ao.png"
            );
            mRockSphere.material()->metallicFactor = 1;
            mRockSphere.material()->roughnessFactor = 1;
            mRockSphere.material()->aoFactor = 1;
            mRockSphere.material()->color = {1, 1, 1, 1 };

            // displacement
            {
                // displace rock sphere
                Image rock_height_map = ImageReader::read("images/bumpy-rockface1-bl/height.png");
                rock_height_map.resize(mRockSphereGeometry.xSegments + 1, mRockSphereGeometry.ySegments + 1);

                mRockSphere.addComponent<DisplacementTBN>();
                auto* rock_sphere_displacement = mRockSphere.getComponent<DisplacementTBN>();
                rock_sphere_displacement->setOriginVertices(&mRockSphereGeometry.vertices);
                rock_sphere_displacement->scale = 1.0f;
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

            mWoodSphere.material()->init(
                    false,
                    "images/cheap-plywood1-bl/albedo.png",
                    "images/cheap-plywood1-bl/normal.png",
                    null,
                    "images/cheap-plywood1-bl/metallic.png",
                    "images/cheap-plywood1-bl/roughness.png",
                    "images/cheap-plywood1-bl/ao.png"
            );
            mWoodSphere.material()->metallicFactor = 1;
            mWoodSphere.material()->roughnessFactor = 1;
            mWoodSphere.material()->aoFactor = 1;

            mMetalSphere.material()->init(
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
    }

    void Application::initApi() {
        mScreenRenderer = new ScreenRenderer();

        mHdrRenderer = new HdrRenderer(mWidth, mHeight);
        mHdrRenderer->isEnabled = true;
        Image shinyImage = ImageReader::read("images/lens_dirt/lens_dirt.png");
        ImageBuffer shinyBuffer;
        shinyBuffer.init();
        shinyBuffer.load(shinyImage);
        shinyImage.free();
        mHdrRenderer->getParams().shinyBuffer = shinyBuffer;
        mHdrRenderer->getParams().exposure.value = 2.0f;
        mHdrRenderer->updateExposure();

        mBlurRenderer = new BlurRenderer(mWidth, mHeight);
        mBlurRenderer->isEnabled = false;

        mBloomRenderer = new BloomRenderer(mWidth, mHeight);
        mBloomRenderer->isEnabled = true;

        mSsaoRenderer = new SsaoRenderer(mWidth, mHeight);
        mSsaoRenderer->isEnabled = true;

        mShadowPipeline = new ShadowPipeline(&mScene, mWidth, mHeight, &mCamera);
        mShadowPipeline->directShadow.filterSize = 18;

        mPbrPipeline = new PBR_Pipeline(&mScene, mWidth, mHeight, mSsaoRenderer);
        mPbrPipeline->setDirectShadow(&mShadowPipeline->directShadow);
        mPbrPipeline->setPointShadow(&mShadowPipeline->pointShadow);
        mPbrPipeline->terrain = &mTerrainBuilder.terrain;

        mUiPipeline = new UI_Pipeline(&mScene, mWidth, mHeight);
        mVisualsPipeline = new VisualsPipeline(&mScene, mWidth, mHeight);
    }

    void Application::initCamera() {
        mCamera.zFar = 1000.0f;
        mCamera.maxPitch = 180;
        mCamera.position = {-5, 2, 10 };
        mCamera.init(0, mWindow->getWidth(), mWindow->getHeight());
    }

    void Application::initLight() {
        // setup light presentation
        CubeDefault().init(mPointLightVisual.drawable);
        // setup environment
        mPbrPipeline->env.resolution = {1024, 1024 };
        mPbrPipeline->env.prefilterResolution = {512, 512 };
        mPbrPipeline->env.init();
        mPbrPipeline->initHdrEnv("images/hdr/Arches_E_PineTree_3k.hdr", true);
        mPbrPipeline->generateEnv();
        // setup mSunlight
        mSunlight = &mScene;
        mSunlight.value().color = {244, 233, 155, 0.01 };
        mSunlight.value().position = {100, 100, 100, 0 };
        mSunlight.direction() = {0, 0, 0, 0 };
        // setup point lights
        for (auto& pointLight : mPointLights) {
            pointLight = &mScene;
        }
        mPointLights[0].value().position = {-4, 2, 0, 1 };
        mPointLights[0].value().color = glm::vec4 {0, 0, 0, 1 };
        mPointLights[1].value().position = {4, 3, 0, 1 };
        mPointLights[1].value().color = glm::vec4 {0, 0, 0, 1 };
        mPointLights[2].value().position = {-4, 4, 8, 1 };
        mPointLights[2].value().color = glm::vec4 {0, 0, 0, 1 };
        mPointLights[3].value().position = {4, 5, 8, 1 };
        mPointLights[3].value().color = glm::vec4 {0, 0, 0, 1 };
        // setup mFlashlight
        mFlashlight = &mScene;
        mFlashlight.value().position = {mCamera.position, 0 };
        mFlashlight.value().direction = {mCamera.front, 0 };
        mFlashlight.value().color = {0, 0, 0, 0 };
        // update light buffers
        mPbrPipeline->updateSunlight(mSunlight.value());
        std::array<PointLightUniform, 4> pointLightUniforms = {
                mPointLights[0].value(),
                mPointLights[1].value(),
                mPointLights[2].value(),
                mPointLights[3].value()
        };
        mPbrPipeline->updatePointLights(pointLightUniforms);
        mPbrPipeline->updateFlashlight(mFlashlight.value());
    }

    void Application::free() {
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

        mCamera.free();

#ifdef IMGUI
        delete mImgui;
#endif

        delete mDevice;
        delete mWindow;

#ifdef DEBUG
        delete mDebugger;
        Logger::free();
#endif
    }

    void Application::simulate() {
        float t = mBeginTime;

        mCamera.move(mWindow, mDeltaTime);
        mPbrPipeline->setCameraPos(mCamera.position);

        // bind flashlight to camera
        mFlashlight.value().position = { mCamera.position, 0 };
        mFlashlight.value().direction = { mCamera.front, 0 };

        // rotate object each frame
        float f = 0.05f;
        mRockSphere.transform()->rotation.y += f;
        mWoodSphere.transform()->rotation.y += f * 2;
        mMetalSphere.transform()->rotation.y += f * 4;
        mHuman.transform()->rotation.y += f * 4;

        // translate sunlight
        float sunlightTranslate = 100 * sin(t);
        mSunlight.value().position.x = sunlightTranslate;
        mSunlight.value().position.z = sunlightTranslate;
        mPbrPipeline->updateSunlight(mSunlight.value());

        // translate point lights up/down
        std::array<PointLightUniform, 4> pointLightUniforms;
        for (int i = 0 ; i < 4 ; i++) {
            auto& pointLight = mPointLights[i];
            pointLight.value().position.y = sin(t/5);
            pointLightUniforms[i] = pointLight.value();
        }
        mPbrPipeline->updatePointLights(pointLightUniforms);

        // skeletal animations
        {
            // animate human
//            mHumanAnimator.update(dt / 1000.0f);
//            pbr.update_bones(mHumanAnimator.bone_transforms);
        }
    }

    void Application::printDt() {
        mPrintLimiter++;
        if (mPrintLimiter > 100) {
            mPrintLimiter = 0;
            printFPS(mDeltaTime)
        }
    }

    void Application::onWindowClose() {
        info("")
    }

    void Application::onWindowResize(int w, int h) {
        info("width: " << w << ", height: " << h)
    }

    void Application::onWindowMove(int x, int y) {
        info("x=" << x << ", y=" << y)
    }

    void Application::onFramebufferResized(int w, int h) {
        info("width=" << w << ", height=" << h)

        mCamera.resize(w, h);
        mPbrPipeline->resize(w, h);
        mUiPipeline->resize(w, h);
        mVisualsPipeline->resize(w, h);
        mHdrRenderer->resize(w, h);
        mBloomRenderer->resize(w, h);
        mBlurRenderer->resize(w, h);
        mShadowPipeline->resize(w, h);
    }

    void Application::onKeyPress(int key) {
        info("")

        if (key == KEY::Esc)
            mWindow->close();

        if (key == KEY::F)
            mWindow->toggleWindowMode();

        if (key == KEY::C)
            mCamera.enableLook = !mCamera.enableLook;
    }

    void Application::onKeyRelease(int key) {
        info("")
    }

    void Application::onMousePress(int button) {
        info("")
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            Cursor mouse_cursor = mWindow->mouseCursor();
            static PBR_Pixel pixel;
            mPbrPipeline->readPixel(pixel, (int) mouse_cursor.x, (int) mouse_cursor.y);
//            info("Read pixel from [x,y] = [" << mouse_cursor.x << "," << mouse_cursor.y << "]")
            mEntityControl->select(mouse_cursor.x, mouse_cursor.y);
        }
    }

    void Application::onMouseRelease(int button) {
        info("")
    }

    void Application::onMouseCursor(double x, double y) {
        mCamera.look(x, y);
        mEntityControl->drag(x, y);
    }

    void Application::onMouseScroll(double x, double y) {
        mCamera.zoom(x, y);
    }

    void Application::onEntitySelected(Entity entity, double x, double y) {
        info("onEntitySelected: [" << x << "," << y << "]")
        auto& selected = entity.getComponent<Selectable>()->enable;
        selected = !selected;
        if (selected) {
            entity.addComponent<PolygonVisual>();
            entity.addComponent<NormalVisual>();
        } else {
            entity.removeComponent<PolygonVisual>();
            entity.removeComponent<NormalVisual>();
        }
    }

    void Application::onEntityDragged(Entity entity, double x, double y) {
        info("onEntityDragged: [" << x << "," << y << "]")
    }

    void Application::onEntityHovered(Entity entity, double x, double y) {
        info("onEntityHovered: [" << x << "," << y << "]")
        entity.getComponent<Material>()->color = { 5, 5, 5, 1 };
    }

    void Application::renderPostFX() {
        // Bloom effect
        if (mBloomRenderer->isEnabled) {
            mBloomRenderer->getHdrBuffer() = mScreenRenderer->getParams().sceneBuffer;
            mBloomRenderer->render();
            mScreenRenderer->getParams().sceneBuffer = mBloomRenderer->getRenderTarget();
        }
        // HDR effect
        if (mHdrRenderer->isEnabled) {
            mHdrRenderer->getParams().sceneBuffer = mScreenRenderer->getParams().sceneBuffer;
            mHdrRenderer->render();
            mScreenRenderer->getParams().sceneBuffer = mHdrRenderer->getParams().sceneBuffer;
        }
        // Blur effect
        if (mBlurRenderer->isEnabled) {
            mBlurRenderer->getParams().sceneBuffer = mScreenRenderer->getParams().sceneBuffer;
            mBlurRenderer->render();
            mScreenRenderer->getParams().sceneBuffer = mBlurRenderer->getParams().sceneBuffer;
        }
    }

    void Application::renderDebugScreen() {
        if (mWindow->isKeyPress(KEY::D1)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::D2)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getGBuffer().position;
        }

        else if (mWindow->isKeyPress(KEY::D3)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getGBuffer().normal;
        }

        else if (mWindow->isKeyPress(KEY::D4)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getGBuffer().albedo;
        }

        else if (mWindow->isKeyPress(KEY::D5)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getGBuffer().pbrParams;
        }

        else if (mWindow->isKeyPress(KEY::D6)) {
            mScreenRenderer->getParams().sceneBuffer = mShadowPipeline->directShadow.map.buffer;
        }

        else if (mWindow->isKeyPress(KEY::D7)) {
            mScreenRenderer->getParams().sceneBuffer = mSsaoRenderer->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::D8)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->env.brdfConvolution;
        }

        else if (mWindow->isKeyPress(KEY::D9)) {
            mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getTransparentBuffer().revealage;
        }

        else if (mWindow->isKeyPress(KEY::D0)) {
            mScreenRenderer->getParams().uiBuffer = mUiPipeline->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::P)) {
            mScreenRenderer->getParams().visualsBuffer = mVisualsPipeline->getRenderTarget();
        }

        else if (mWindow->isKeyPress(KEY::T)) {
            mScreenRenderer->getParams().sceneBuffer = mFontRobotoRegular->buffer;
        }
    }

    void Application::render() {
        mShadowPipeline->render();

        mPbrPipeline->render();
        mScreenRenderer->getParams().sceneBuffer = mPbrPipeline->getRenderTarget();

        mUiPipeline->render();
        mScreenRenderer->getParams().uiBuffer = mUiPipeline->getRenderTarget();

#ifdef DEBUG
        mVisualsPipeline->render();
        mScreenRenderer->getParams().visualsBuffer = mVisualsPipeline->getRenderTarget();
#endif

        renderPostFX();

        renderDebugScreen();

        mScreenRenderer->render();

#ifdef IMGUI
        renderImgui();
#endif
    }

    void Application::initText() {
        FontAtlas::init();

        mFontRobotoRegular = FontAtlas::load("fonts/Roboto-Regular.ttf", 40);
        mFontRobotoRegular->saveBmp("fonts/Roboto-Regular.bmp");
        mFontRobotoRegular->saveWidths("fonts/Roboto-Regular.widths");

        mTextLabel = &mScene;
        mTextLabel.addComponent<Text3d>(mFontRobotoRegular, "Hello World!");
        mTextLabel.getComponent<Text3d>()->transform.translation = {0, 5, 4 };
    }

    void Application::renderImgui() {
        mImgui->begin();

        ScreenProperties::render();

        mImgui->end();
    }
}