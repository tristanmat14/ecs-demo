#include "WinApplication.hpp"

int main(int argc, char* argv[]) {
    WinApplication app; 

    if (app.start()) {
        app.run();
    }
    
    app.end();

    return 0;
}
