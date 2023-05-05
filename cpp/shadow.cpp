#include <shadow.h>
#include <light.h>

namespace gl {

    ShadowPipeline::ShadowPipeline(ecs::Scene* scene, int width, int height, Camera* camera) : scene(scene) {
        mDirectShadowRenderer = new DirectShadowRenderer();
        mPointShadowRenderer = new PointShadowRenderer();

        createDirectShadow(width, height, camera);
        createPointShadow(width, height, camera);

        mFrame.init();
        mFrame.depth = directShadow.map;
        mFrame.attach_depth();
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
        directShadow.map.image.pixel_type = PixelType::FLOAT;
        directShadow.map.params.min_filter = GL_NEAREST;
        directShadow.map.params.mag_filter = GL_NEAREST;
        directShadow.map.params.s = GL_CLAMP_TO_EDGE;
        directShadow.map.params.t = GL_CLAMP_TO_EDGE;
        directShadow.map.params.r = GL_CLAMP_TO_EDGE;
        directShadow.map.params.border_color = { 1, 1, 1, 1 };
        directShadow.map.init();
    }

    void ShadowPipeline::createPointShadow(int width, int height, Camera* camera) {
        pointShadow.camera = camera;
        pointShadow.fov_degree = 90.0f;
        pointShadow.aspect_ratio = (float) width / (float) height;
        pointShadow.zNear = 1.0f;
        pointShadow.zFar = 25.0f;
        pointShadow.map.buffer.type = GL_TEXTURE_CUBE_MAP;
        pointShadow.map.image = { width, height };
        pointShadow.map.image.pixel_type = PixelType::FLOAT;
        pointShadow.map.params.min_filter = GL_NEAREST;
        pointShadow.map.params.mag_filter = GL_NEAREST;
        pointShadow.map.params.s = GL_CLAMP_TO_EDGE;
        pointShadow.map.params.t = GL_CLAMP_TO_EDGE;
        pointShadow.map.params.r = GL_CLAMP_TO_EDGE;
        pointShadow.map.params.border_color = { 1, 1, 1, 1 };
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
        mFrame.attach_depth();
        Viewport::resize(0, 0, shadowMap.image.width, shadowMap.image.height);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void ShadowPipeline::renderDirectShadows() {
        directShadow.lightSpaces.clear();

        scene->each_component<DirectLightComponent>([this](DirectLightComponent* directLightComponent) {
            glm::vec3 lightDirection = directLightComponent->value.direction;

            mDirectShadowRenderer->begin();

            scene->each_component<Shadowable>([this, &lightDirection](Shadowable* shadowable) {
                ecs::EntityID entityId = shadowable->entity_id;
                mDirectShadowRenderer->render(
                        *scene->get_component<Transform>(entityId),
                        *scene->get_component<DrawableElements>(entityId),
                        directShadow,
                        lightDirection
                );
            });

            mDirectShadowRenderer->end();
        });
    }

    void ShadowPipeline::renderPointShadows() {
        scene->each_component<PointLightComponent>([this](PointLightComponent* pointLightComponent) {
            glm::vec3 lightPosition = pointLightComponent->value.position;

            mPointShadowRenderer->begin();

            scene->each_component<Shadowable>([this, &lightPosition](Shadowable* shadowable) {
                ecs::EntityID entityId = shadowable->entity_id;
                mPointShadowRenderer->render(
                        *scene->get_component<Transform>(entityId),
                        *scene->get_component<DrawableElements>(entityId),
                        pointShadow,
                        lightPosition
                );
            });

            mPointShadowRenderer->end();
        });
    }

}