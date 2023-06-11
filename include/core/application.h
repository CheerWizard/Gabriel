#pragma once

#include <core/window.h>
#include <core/imgui_core.h>
#include <core/timer.h>

#include <debugging/visuals.h>

#include <control/camera.h>
#include <control/entity_control.h>

#include <ui/ui.h>

#include <features/lighting/light.h>
#include <features/screen.h>
#include <features/shadow/shadow.h>

#include <io/model_loader.h>

#include <math/maths.h>

#include <geometry/sphere.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>

#include <pbr/pbr.h>

#include <animation/skeletal_animation.h>

#include <terrain/terrain.h>

#include <raytracer/raytracer.h>

#include <imgui/toolbar.h>
#include <imgui/screen_window.h>
#include <imgui/properties_window.h>
#include <imgui/entity_window.h>
#include <imgui/component_window.h>
#include <imgui/gizmo.h>

namespace gl {

    class Application : ImguiCoreCallback {

    public:
        Application(
                const char* title,
                int width, int height,
                const char* logoName,
                const ThemeMode themeMode = DARK_MODE
        );
        ~Application();

        void run();

        void onWindowClose();
        void onWindowMove(const int x, const int y);
        void onWindowResize(const int w, const int h);
        void onFramebufferResized(const int w, const int h);

        void onKeyPress(const KEY key);
        void onKeyRelease(const KEY key);

        void onMousePress(const int button);
        void onMouseRelease(const int button);
        void onMouseCursor(const double x, const double y);
        void onMouseScroll(const double x, const double y);

        void resize(int w, int h) override;
        void resample(int samples) override;

    private:
        void initLogger();
        void initWindow();
        void initScene();
        void initApi();
        void initCamera();
        void initLight();
        void initText();
        void initImgui();
        void initEnvironment();

        void printDt();

        static void onEntitySelected(Entity entity, double x, double y);
        static void onEntityDragged(Entity entity, double x, double y);
        static void onEntityHovered(Entity entity, double x, double y);

        void updateInput();

        void simulate();

        void renderPostFX();

        void renderImgui();

        void render();

    private:
        const char* mTitle;
        const char* mLogoName;
        int mWidth;
        int mHeight;
        bool mRunning = true;
        ThemeMode mThemeMode;

        Window* mWindow = null;
        Device* mDevice = null;
        Debugger* mDebugger = null;

        Scene mScene = "Gabriel";

        Camera* mCamera = null;

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
        FXAARenderer* mFxaaRenderer;
        TransparentRenderer* mTransparentRenderer;

        Animator mHumanAnimator;
        SkeletalModel mHumanModel;
        PBR_Entity mHuman;

        SphereTBN mRockSphereGeometry = {256, 256 };
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

        RayTraceRenderer* mRayTraceRenderer = null;

        Environment mEnvironment;
        EnvRenderer* mEnvRenderer = null;

        OutlineRenderer* mOutlineRenderer = null;

        Ray mRay = { 0, 0, 0 };
        Sphere mSphere;

        FrameBuffer mColorFrame;
        FrameBuffer mDepthFrame;

        ImageBuffer mFinalRenderTarget;
        ImageBuffer mDebugRenderTarget;
    };

}