#include "App.h"
#include <iostream>

int main() {
    App app(800, 600, "Snake OpenGL v1.0");

    if (!app.init()) {
        return -1;
    }
    app.run();
    return 0;
}