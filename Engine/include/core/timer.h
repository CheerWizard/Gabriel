#pragma once

namespace gl {

    struct GABRIEL_API Timer final {

        static void begin();
        static void end();

        static Time getBeginMillis();
        static Time getDeltaMillis();
        static Time getFPS();
        static std::string getCurrentDateTime();

    private:
        static Time s_beginTime;
        static Time s_deltaTime;
    };

}