#include <light.h>

namespace gl {

    static Shader lightPresentShader;
    static DrawableElements lightPresentDrawable;

    void LightPresent::init() {
        lightPresentShader.add_vertex_stage("shaders/light_present.vert");
        lightPresentShader.add_fragment_stage("shaders/light_present.frag");
        lightPresentShader.complete();

        presentation.init(lightPresentDrawable);
    }

    void LightPresent::free() {
        lightPresentShader.free();
        lightPresentDrawable.free();
    }

    void LightPresent::update() {
        lightPresentShader.use();
        lightPresentShader.set_uniform_args("object_id", id);
        lightPresentShader.set_uniform_args("draw_id", id);
        lightPresentShader.set_uniform_args("color", color);
        transform.update(lightPresentShader);
        lightPresentDrawable.draw();
    }

    PhongLightUniform& PhongLight::value() {
        return get_component<PhongLightComponent>()->value;
    }

    DirectLightUniform& DirectLight::value() {
        return get_component<DirectLightComponent>()->value;
    }

    PointLightUniform& PointLight::value() {
        return get_component<PointLightComponent>()->value;
    }

    SpotLightUniform& SpotLight::value() {
        return get_component<SpotLightComponent>()->value;
    }

}