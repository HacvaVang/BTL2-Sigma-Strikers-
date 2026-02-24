#include "../include/SDLFramework.h"
#include <iostream>
#include "../include/Menu.h"

int main(int argc, char** argv) {
    SDLFramework app;
    if (!app.init("Sigma Strikers - SDL Framework", 1024, 768)) {
        std::cerr << "Failed to initialize SDL framework" << std::endl;
        return 1;
    }

    // Show resolution menu before entering main loop (use arrow keys + Enter)
    showResolutionMenu(app);

    app.run();
    return 0;
}
