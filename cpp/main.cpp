#include <core/application.h>

int main() {
    gl::Application(
            "Gabriel",
            800, 600,
            "Gabriel_Logo.png",
            gl::ThemeMode::DARK_MODE
    ).run();
    return 0;
}
