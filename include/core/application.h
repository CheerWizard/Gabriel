#pragma once

#include <core/window.h>
#include <core/imgui_core.h>

#include <debugging/visuals.h>

#include <control/camera.h>
#include <control/entity_control.h>

#include <ui/ui.h>

#include <features/light.h>
#include <features/screen.h>

#include <io/model_loader.h>

#include <shadow/shadow.h>

#include <math/math_functions.h>

#include <geometry/sphere.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>

#include <pbr/pbr.h>

#include <animation/skeletal_animation.h>

#include <terrain/terrain.h>

#include <imgui/screen_properties.h>

namespace gl {

    class Application final {

    public:
        Application(const char* title, int width, int height);
        ~Application();

        void run();

        void onWindowClose();
        void onWindowMove(int x, int y);
        void onWindowResize(int w, int h);
        void onFramebufferResized(int w, int h);

        void onKeyPress(int key);
        void onKeyRelease(int key);

        void onMousePress(int button);
        void onMouseRelease(int button);
        void onMouseCursor(double x, double y);
        void onMouseScroll(double x, double y);

    private:
        void initLogger();
        void initWindow();
        void initScene();
        void initApi();
        void initCamera();
        void initLight();
        void initText();
        void initImgui();

        void free();

        void printDt();

        static void onEntitySelected(Entity entity, double x, double y);
        static void onEntityDragged(Entity entity, double x, double y);
        static void onEntityHovered(Entity entity, double x, double y);

        void simulate();

        void renderPostFX();

        void renderDebugScreen();

        void renderImgui();

        void render();

    private:
        const char* mTitle;
        int mWidth;
        int mHeight;
        bool mRunning = true;
        float mDeltaTime = 6;
        float mBeginTime = 0;

        Window* mWindow = null;
        Device* mDevice = null;
        Debugger* mDebugger = null;

        Scene mScene;

        Camera mCamera;

        LightVisual mPointLightVisual;

        TerrainBuilder mTerrainBuilder;

        DisplacementImageMixer mImageMixer;

        Model mBackpackModel;
        PBR_Entity mBackpack;

        PBR_Pipeline* mPbrPipeline;
        UI_Pipeline* mUiPipeline;
        VisualsPipeline* mVisualsPipeline;
        ShadowPipeline* mShadowPipeline;

        ScreenRenderer* mScreenRenderer;
        HdrRenderer* mHdrRenderer;
        BloomRenderer* mBloomRenderer;
        BlurRenderer* mBlurRenderer;
        SsaoRenderer* mSsaoRenderer;

        Animator mHumanAnimator;
        SkeletalModel mHumanModel;
        PBR_Entity mHuman;

        SphereTBN mRockSphereGeometry = {128, 128 };
        PBR_Entity mRockSphere;

        PBR_Entity mWoodSphere;

        PBR_Entity mMetalSphere;

        int mPrintLimiter = 100;

        EntityControl* mEntityControl;

        Entity mTextLabel;
        Font* mFontRobotoRegular = null;

        DirectLight mSunlight;
        std::array<PointLight, 4> mPointLights;
        SpotLight mFlashlight;

        ImguiCore* mImgui = null;
    };

}