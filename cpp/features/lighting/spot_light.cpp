#include <features/lighting/spot_light.h>

namespace gl {

    SpotLightComponent& SpotLight::value() {
        return *getComponent<SpotLightComponent>();
    }

}