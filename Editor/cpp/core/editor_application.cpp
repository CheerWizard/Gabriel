#include <core/editor_application.h>
#include <core/stbi_core.h>

namespace gl {

    Application* createApplication() {
        return new EditorApplication(
                "Gabriel",
                800, 600,
                "Gabriel_Logo.png",
                gl::ThemeMode::DARK_MODE
        );
    }

    void EditorApplication::onCreate() {
        Application::onCreate();

        info("onCreate()");

        loadHdrShinyImage("Assets/images/lens_dirt/lens_dirt.png");

        initCamera();

        initEnvironment();

        initScene();

        initLight();

        initText();
    }

    void EditorApplication::onCreateImgui() {
        Application::onCreateImgui();
    }

    void EditorApplication::initScene() {
        setScene(new Scene("Untitled"));

        mBackpack = {
                mScene,
                { -3, 1.5, 5 },
                { -90, 0, 0 },
                { 1, 1, 1 }
        };

        mTerrainBuilder = {
                mScene,
                { 0, 0.1, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 },
                32
        };

        mHuman = {
                mScene,
                { 0, 0, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mRockSphere = {
                mScene,
                { 0, 1.5, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mWoodSphere = {
                mScene,
                { 0, 1.5, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mMetalSphere = {
                mScene,
                { 0, 1.5, 8 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        mEntityControl = new EntityControl(mScene, mCamera);

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
        mHuman.addComponent<Shadowable>();

        // setup 3D model
        mBackpackModel.generate("Assets/models/backpack/backpack.obj");
        mBackpackModel.init(*mBackpack.getComponent<DrawableElements>());
//        model_shadow.init(model);
        mBackpack.material()->load(
                true,
                "Assets/models/backpack/diffuse.jpg",
                "Assets/models/backpack/normal.png",
                null,
                "Assets/models/backpack/specular.jpg",
                "Assets/models/backpack/roughness.jpg",
                "Assets/models/backpack/ao.jpg"
        );
        mBackpack.material()->metallicFactor = 1;
        mBackpack.material()->roughnessFactor = 0.5;
        mBackpack.material()->aoFactor = 1;

        // setup mHuman model
        mHumanModel.generate("Assets/models/dancing-stormtrooper/source/silly_dancing.fbx");
        mHumanModel.init(*mHuman.getComponent<DrawableElements>());
        mHuman.material()->load(
                false,
                "Assets/models/dancing-stormtrooper/textures/Stormtrooper_D.png",
                null,
                null,
                null,
                null,
                null
        );
        mHuman.material()->metallicFactor = 0.8;
        mHuman.material()->roughnessFactor = 0.2;
        mHuman.material()->aoFactor = 1.0;
        mHumanAnimator = Animator(&mHumanModel.animation);

        // setup horizontal plane
        mTerrainBuilder.terrain.material.load(
                false,
                null,
                "Assets/images/bumpy-rockface1-bl/normal.png",
                null,
                "Assets/images/bumpy-rockface1-bl/metallic.png",
                "Assets/images/bumpy-rockface1-bl/roughness.png",
                "Assets/images/bumpy-rockface1-bl/ao.png"
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
                    "Assets/images/columned_lava/albedo.png",
                    "Assets/images/columned_lava/normal.png",
                    null,
                    "Assets/images/columned_lava/metallic.png",
                    "Assets/images/columned_lava/roughness.png",
                    "Assets/images/columned_lava/ao.png",
                    "Assets/images/columned_lava/emission.png"
            );
            mRockSphere.material()->metallicFactor = 1;
            mRockSphere.material()->roughnessFactor = 1;
            mRockSphere.material()->aoFactor = 1;
            mRockSphere.material()->emissionFactor = { 1, 1, 1 };

            // displacement
            {
                // displace rock sphere
                Image rock_height_map = ImageReader::read("Assets/images/columned_lava/height.png");
                rock_height_map.resize(mRockSphereGeometry.xSegments + 1, mRockSphereGeometry.ySegments + 1);

                mRockSphere.addComponent<DisplacementTBN>();
                auto* rock_sphere_displacement = mRockSphere.getComponent<DisplacementTBN>();
                rock_sphere_displacement->setOriginVertices(&mRockSphereGeometry.vertices);
                rock_sphere_displacement->scale = 0.1f;
                rock_sphere_displacement->map = HeightMap(rock_height_map);
                rock_sphere_displacement->displace(*mRockSphere.getComponent<DrawableElements>());

                rock_height_map.free();

                // displace terrain
                Image terrain_height_map = ImageReader::read("Assets/images/terrain/earth_heightmap.png");
                terrain_height_map.resize(mTerrainBuilder.size(), mTerrainBuilder.size());

                mTerrainBuilder.displaceWith(MidPointFormation(mTerrainBuilder.size(), mTerrainBuilder.size(), 10, 0, 1), 1);
                mImageMixer.displacementMap = &mTerrainBuilder.displacement.map;
                mImageMixer.addImage({-1.0, -0.5, 0 }, "Assets/images/terrain/rock_tile.png");
                mImageMixer.addImage({0, 0.1, 0.2 }, "Assets/images/terrain/sand_tile.jpg");
                mImageMixer.addImage({0.20, 0.45, 0.70 }, "Assets/images/terrain/grass_tile.png");
                mImageMixer.addImage({0.70, 0.77, 0.85 }, "Assets/images/terrain/rock_tile.png");
                mImageMixer.addImage({0.85, 0.90, 1.0 }, "Assets/images/terrain/snow_tile.png");
                mImageMixer.mix(mTerrainBuilder.size(), mTerrainBuilder.size());
                ImageWriter::write("Assets/images/terrain/mixed_image.png", mImageMixer.mixedImage);

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
                    "Assets/images/cheap-plywood1-bl/normal.png",
                    null,
                    "Assets/images/cheap-plywood1-bl/metallic.png",
                    "Assets/images/cheap-plywood1-bl/roughness.png",
                    "Assets/images/cheap-plywood1-bl/ao.png"
            );
            mWoodSphere.material()->color = { 1, 1, 1, 0.5 };
            mWoodSphere.material()->metallicFactor = 1;
            mWoodSphere.material()->roughnessFactor = 1;
            mWoodSphere.material()->aoFactor = 1;

            mMetalSphere.material()->load(
                    false,
                    "Assets/images/light-gold-bl/albedo.png",
                    "Assets/images/light-gold-bl/normal.png",
                    null,
                    "Assets/images/light-gold-bl/metallic.png",
                    "Assets/images/light-gold-bl/roughness.png",
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

    void EditorApplication::initEnvironment() {
        mEnvironment = new Environment();
        mEnvironment->enable = true;
        mEnvironment->resolution = { 1024, 1024 };
        mEnvironment->prefilterResolution = { 512, 512 };
        mEnvironment->hdr.loadHDR("Assets/images/hdr/Arches_E_PineTree_3k.hdr", true);
        mEnvironment->init();
        setEnvironment(mEnvironment);
    }

    void EditorApplication::initCamera() {
        mCamera = new Camera(0, mWindow);
        mCamera->zFar = 1000.0f;
        mCamera->position = {-5, 2, 10 };
        setCamera(mCamera);
    }

    void EditorApplication::initLight() {
        // setup light presentation
        CubeDefault().init(mPointLightVisual.drawable);
        // setup mSunlight
        mSunlight = mScene;
        mSunlight.value().color = { 244, 233, 155, 0.1f };
        mSunlight.value().position = { 0, 10, 0, 0 };
        mSunlight.value().direction = { 0, 0, 0 };
        // setup point lights
        for (auto& pointLight : mPointLights) {
            pointLight = mScene;
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
        mFlashlight = mScene;
        mFlashlight.value().position = { mCamera->position, 0 };
        mFlashlight.value().direction = { mCamera->front, 0 };
        mFlashlight.value().color = {0, 0, 0, 0 };

        LightStorage::update();
    }

    void EditorApplication::initText() {
        mFontRobotoRegular = FontAtlas::load("Assets/fonts/Roboto-Regular.ttf", 40);
//        mFontRobotoRegular->saveBmp("Assets/fonts/Roboto-Regular.bmp");
//        mFontRobotoRegular->saveWidths("Assets/fonts/Roboto-Regular.widths");

        mTextLabel = mScene;
        mTextLabel.addComponent<Text3d>(mFontRobotoRegular, "Hello World!");
        mTextLabel.getComponent<Text3d>()->transform.translation = { 0, 5, 0 };
        mTextLabel.getComponent<Text3d>()->transform.init();
    }

    void EditorApplication::onDestroy() {
        info("onDestroy()");

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

        delete mCamera;

        Application::onDestroy();
    }

    void EditorApplication::onUpdateInput(const float dt) {
        Application::onUpdateInput(dt);

#ifdef IMGUI
        if (!ImguiCore::IO->WantTextInput) {
            mCamera->move(Timer::getDeltaMillis());
        }
#else
        mCamera->move(Timer::getDeltaMillis());
#endif
    }

    void EditorApplication::onSimulate(const float dt) {
        Application::onSimulate(dt);

        // bind flashlight to camera
        mFlashlight.value().position = { mCamera->position, 0 };
        mFlashlight.value().direction = { mCamera->front, 0 };

        // rotate object each frame
        float f = 0.05f;
        mRockSphere.transform()->rotation.y += f;
        mWoodSphere.transform()->rotation.y += f * 2;
        mMetalSphere.transform()->rotation.y += f * 4;
        mHuman.transform()->rotation.y += f * 4;

        mRockSphere.transform()->init();
        mWoodSphere.transform()->init();
        mMetalSphere.transform()->init();
        mHuman.transform()->init();

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

    void EditorApplication::onRender(const float dt) {
        Application::onRender(dt);
    }

    void EditorApplication::onRenderPostFX(const float dt) {
        Application::onRenderPostFX(dt);
    }

    void EditorApplication::onRenderImgui(const float dt) {
        Application::onRenderImgui(dt);
        MainWindow::render();
        ScreenWindow::render();
        PropertiesWindow::render();
        EntityWindow::render();
        ComponentWindow::render();
    }

    void EditorApplication::onKeyPress(const KEY key) {
        Application::onKeyPress(key);

        switch (key) {
            case T:
                mDebugRenderTarget = mFontRobotoRegular->buffer;
                break;

            default:
                break;
        }
    }

    void EditorApplication::onMouseCursor(const double x, const double y) {
        Application::onMouseCursor(x, y);

#ifdef IMGUI
        if (!ImguiCore::IO->WantCaptureMouse) {
            mCamera->onMouseCursor(x, y, Timer::getDeltaMillis());
            mEntityControl->drag(x, y);
        }
#else
        mCamera->onMouseCursor(x, y, Timer::getDeltaMillis());
        mEntityControl->drag(x, y);
#endif

    }

    void EditorApplication::onMouseScroll(const double x, const double y) {
        Application::onMouseScroll(x, y);

#ifdef IMGUI
        if (!ImguiCore::IO->WantCaptureMouse) {
            mCamera->onMouseScroll(y, Timer::getDeltaMillis());
        }
#else
        mCamera->onMouseScroll(y, Timer::getDeltaMillis());
#endif

    }

}