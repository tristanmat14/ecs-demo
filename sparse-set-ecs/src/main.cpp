#include "Application.hpp"

int main(int argc, char* argv[]) {
    Application app; 

    app.start();

    for (int i = 0; i < 1000; ++i) {
        app.runFrame();
    }

    app.end();

    return 0;
}
