#include <features/shadow/shadow.h>
#include <features/lighting/light.h>

namespace gl {

    ShadowPipeline::ShadowPipeline(Scene* scene, int width, int height, Camera* camera) : scene(scene) {
        mDirectShadowRenderer = new DirectShadowRenderer();
        mPointShadowRenderer = new PointShadowRenderer();

        createDirectShadow(width, height, camera);
        createPointShadow(width, height, camera);

        mFrame.init();
        mFrame.depth = directShadow.map;
        mFrame.attachDepth();
        mFrame.complete();
    }

    void ShadowPipeline::createDirectShadow(int width, int height, Camera* camera) {
        directShadow.camera = camera;
        directShadow.left = -10.0f;
        directShadow.right = 10.0f;
        directShadow.bottom = -10.0f;
        directShadow.top = 10.0f;
        directShadow.zNear = 1.0f;
        directShadow.zFar = 25.0f;
        directShadow.map.image = { width, height };
        directShadow.map.image.pixelType = PixelType::FLOAT;
        directShadow.map.params.minFilter = GL_NEAREST;
        directShadow.map.params.magFilter = GL_NEAREST;
        directShadow.map.params.s = GL_CLAMP_TO_EDGE;
        directShadow.map.params.t = GL_CLAMP_TO_EDGE;
        directShadow.map.params.r = GL_CLAMP_TO_EDGE;
        directShadow.map.params.borderColor = { 1, 1, 1, 1 };
        directShadow.map.init();
    }

    void ShadowPipeline::createPointShadow(int width, int height, Camera* camera) {
        pointShadow.camera = camera;
        pointShadow.fovDegree = 90.0f;
        pointShadow.aspectRatio = (float) width / (float) height;
        pointShadow.zNear = 1.0f;
        pointShadow.zFar = 25.0f;
        pointShadow.map.buffer.type = GL_TEXTURE_CUBE_MAP;
        pointShadow.map.image = { width, height };
        pointShadow.map.image.pixelType = PixelType::FLOAT;
        pointShadow.map.params.minFilter = GL_NEAREST;
        pointShadow.map.params.magFilter = GL_NEAREST;
        pointShadow.map.params.s = GL_CLAMP_TO_EDGE;
        pointShadow.map.params.t = GL_CLAMP_TO_EDGE;
        pointShadow.map.params.r = GL_CLAMP_TO_EDGE;
        pointShadow.map.params.borderColor = { 1, 1, 1, 1 };
        pointShadow.map.init();
    }

    ShadowPipeline::~ShadowPipeline() {
        mFrame.free();
        delete mDirectShadowRenderer;
        delete mPointShadowRenderer;
    }

    void ShadowPipeline::resize(int width, int height) {
        mFrame.resize(width, height);

        directShadow.map.image.width = width;
        directShadow.map.image.height = height;

        pointShadow.map.image.width = width;
        pointShadow.map.image.height = height;
    }

    void ShadowPipeline::render() {
        begin();

        bind(directShadow.map);
        renderDirectShadows();

//        bind(pointShadow.map);
//        renderPointShadows();

        end();
    }

    void ShadowPipeline::begin() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        mFrame.bindWriting();
    }

    void ShadowPipeline::end() {
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    void ShadowPipeline::bind(const DepthAttachment& shadowMap) {
        mFrame.depth = shadowMap;
        mFrame.attachDepth();
        Viewport::resize(0, 0, shadowMap.image.width, shadowMap.image.height);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void ShadowPipeline::renderDirectShadows() {
        directShadow.lightSpaces.clear();

        scene->eachComponent<DirectLightComponent>([this](DirectLightComponent* directLightComponent) {
            glm::vec3 lightDirection = directLightComponent->direction;
            glm::vec3 lightPos = directLightComponent->position;

            mDirectShadowRenderer->begin();

            scene->eachComponent<Shadowable>([this, &lightDirection, &lightPos](Shadowable* shadowable) {
                EntityID entityId = shadowable->entityId;
                mDirectShadowRenderer->render(
                        *scene->getComponent<Transform>(entityId),
                        *scene->getComponent<DrawableElements>(entityId),
                        directShadow,
                        lightPos,
                        lightDirection
                );
            });

            mDirectShadowRenderer->end();
        });
    }

    void ShadowPipeline::renderPointShadows() {
        scene->eachComponent<PointLightComponent>([this](PointLightComponent* pointLightComponent) {
            glm::vec3 lightPosition = pointLightComponent->position;

            mPointShadowRenderer->begin();

            scene->eachComponent<Shadowable>([this, &lightPosition](Shadowable* shadowable) {
                EntityID entityId = shadowable->entityId;
                mPointShadowRenderer->render(
                        *scene->getComponent<Transform>(entityId),
                        *scene->getComponent<DrawableElements>(entityId),
                        pointShadow,
                        lightPosition
                );
            });

            mPointShadowRenderer->end();
        });
    }

}