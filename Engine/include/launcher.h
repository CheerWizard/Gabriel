#pragma once

#include <core/application.h>

int main(int argc, char** argv) {
    auto* application = gl::createApplication();
    application->run();
    delete application;

    return 0;
}