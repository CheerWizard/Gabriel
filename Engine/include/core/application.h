#pragma once

#include <core/window.h>
#include <core/imgui_core.h>
#include <core/timer.h>

#include <debugging/debugger.h>
#include <debugging/visuals.h>

#include <control/camera.h>
#include <control/entity_control.h>

#include <ui/ui.h>

#include <features/lighting/light.h>
#include <features/screen.h>
#include <features/shadow/shadow.h>

#include <io/model_loader.h>
#include <io/image_loader.h>

#include <math/maths.h>

#include <geometry/sphere.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>

#include <pbr/pbr.h>

#include <animation/skeletal_animation.h>

#include <terrain/terrain.h>

#include <raytracer/raytrace_renderer.h>
#include <raytracer/raytrace_storage.h>

#include <imgui/toolbar.h>
#include <imgui/screen_window.h>
#include <imgui/properties_window.h>
#include <imgui/entity_window.h>
#include <imgui/component_window.h>
#include <imgui/gizmo.h>

#include <network/services/database_service.h>

namespace gl {

    class Application;

    Application* createApplication();

    class GABRIEL_API Application : public ImguiCoreCallback {

    public:
        Application(
                const char* title,
                int width, int height,
                const char* logoName,
                const ThemeMode themeMode = DARK_MODE
        );
        virtual ~Application();

        void run();

        virtual void onWindowClose();
        virtual void onWindowMove(const int x, const int y);
        virtual void onWindowResize(const int w, const int h);
        virtual void onFramebufferResized(const int w, const int h);

        virtual void onKeyPress(const KEY key);
        virtual void onKeyRelease(const KEY key);

        virtual void onMousePress(const int button);
        virtual void onMouseRelease(const int button);
        virtual void onMouseCursor(const double x, const double y);
        virtual void onMouseScroll(const double x, const double y);

        void resize(int w, int h) override;
        void resample(int samples) override;

    protected:
        virtual void onCreate();
        virtual void onCreateImgui();
        virtual void onDestroy();
        virtual void onRender(const float dt);
        virtual void onRenderPostFX(const float dt);
        virtual void onRenderImgui(const float dt);
        virtual void onSimulate(const float dt);
        virtual void onUpdateInput(const float dt);

        static void onEntitySelected(Entity entity, double x, double y);
        static void onEntityDragged(Entity entity, double x, double y);
        static void onEntityHovered(Entity entity, double x, double y);

        void loadHdrShinyImage(const char* filepath);
        void setScene(Scene* scene);
        void setEnvironment(Environment* environment);
        void setCamera(Camera* camera);

    private:
        void initLogger();
        void initWindow();
        void initApi();
        void initNetwork();
        void initImgui();

        void renderImgui(const float dt);

        void printDt();

    protected:
        const char* mTitle;
        const char* mLogoName;
        int mWidth;
        int mHeight;
        bool mRunning = false;
        ThemeMode mThemeMode;

        Scene* mScene = null;
        Environment* mEnvironment = null;
        Camera* mCamera = null;

        Window* mWindow = null;
        Device* mDevice = null;
        Debugger* mDebugger = null;

        TerrainBuilder mTerrainBuilder;
        DisplacementImageMixer mImageMixer;

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

        int mPrintLimiter = 100;

        EntityControl* mEntityControl;

        RayTraceRenderer* mRayTraceRenderer = null;

        EnvRenderer* mEnvRenderer = null;

        OutlineRenderer* mOutlineRenderer = null;

        FrameBuffer mColorFrame;
        FrameBuffer mDepthFrame;

        ImageBuffer mFinalRenderTarget;
        ImageBuffer mDebugRenderTarget;

        TCPClient* mTcpClient = null;
        DatabaseService* mDatabaseService = null;
    };

}