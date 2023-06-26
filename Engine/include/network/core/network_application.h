#pragma once

namespace gl {

    struct GABRIEL_API NetworkApplication {

        NetworkApplication(const char* title, const char* ip, const u16 port);

        virtual ~NetworkApplication();

        virtual void run();

    protected:
        const char* m_title = null;
        const char* m_ip = null;
        u16 m_port = 0;
    };

    NetworkApplication* createNetworkApplication(int argc, char** argv);

}