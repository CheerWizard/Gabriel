#include <features/lighting/point_light.h>

namespace gl {

    PointLightComponent& PointLight::value() {
        return *getComponent<PointLightComponent>();
    }

}