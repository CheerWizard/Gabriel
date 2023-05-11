#include <features/lighting/direct_light.h>

namespace gl {

    DirectLightComponent& DirectLight::value() {
        return *getComponent<DirectLightComponent>();
    }

}