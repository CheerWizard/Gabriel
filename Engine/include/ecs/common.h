#pragma once

#include <ecs/component.h>

namespace gl {

    component(Tag) {
        std::string buffer;

        Tag(const char* buffer) : buffer(buffer) {}
        Tag(const std::string& buffer) : buffer(buffer) {}
        ~Tag() = default;
    };

}