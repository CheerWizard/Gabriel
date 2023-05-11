#include <features/lighting/phong_light.h>

namespace gl {

    PhongLightComponent& PhongLight::value() {
        return *getComponent<PhongLightComponent>();
    }

}