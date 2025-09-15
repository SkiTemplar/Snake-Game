#include "App.h"

/**
 * @brief Punto de entrada. Crea la aplicación, la inicializa y ejecuta.
 */
int main() {
    App app(800, 600, "Snake OpenGL v1.0");
    if (!app.init()) return -1;
    app.run();
    return 0;
}
