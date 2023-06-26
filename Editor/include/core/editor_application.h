#pragma once

#include <core/application.h>

namespace gl {

    struct EditorApplication : public Application {

        EditorApplication(
                const char* title,
                int width, int height,
                const char* logoName,
                const ThemeMode themeMode = DARK_MODE
        ) : Application(title, width, height, logoName, themeMode) {}

    protected:
        void onCreate() override;
        void onDestroy() override;

        void onUpdateInput(const float dt) override;
        void onSimulate(const float dt) override;
        void onRender(const float dt) override;

        void onRenderImgui(const float dt) override;

        void onRenderPostFX(const float dt) override;

        void onCreateImgui() override;

    public:
        void onKeyPress(const KEY key) override;

        void onMouseCursor(const double x, const double y) override;

        void onMouseScroll(const double x, const double y) override;

    private:
        void initScene();
        void initCamera();
        void initLight();
        void initText();
        void initEnvironment();

    private:
        LightVisual mPointLightVisual;

        Model mBackpackModel;
        PBR_Entity mBackpack;

        Ray mRay = { 0, 0, 0 };
        Sphere mSphere;

        DirectLight mSunlight;
        std::array<PointLight, 4> mPointLights;
        SpotLight mFlashlight;

        Entity mTextLabel;
        Font* mFontRobotoRegular = null;

        Animator mHumanAnimator;
        SkeletalModel mHumanModel;
        PBR_Entity mHuman;

        SphereTBN mRockSphereGeometry = {256, 256 };
        PBR_Entity mRockSphere;

        PBR_Entity mWoodSphere;

        PBR_Entity mMetalSphere;
    };

}