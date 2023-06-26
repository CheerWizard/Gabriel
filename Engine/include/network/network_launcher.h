#pragma once

#include <network/core/network_application.h>

int main(int argc, char** argv) {
    auto* application = gl::createNetworkApplication(argc, argv);
    application->run();
    delete application;

    return 0;
}