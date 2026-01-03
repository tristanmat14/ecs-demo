#include "Win32Core.hpp"
#include "ECSApplication.hpp"

int main(int argc, char* argv[]) {
    auto core = std::make_unique<Win32Core>();
    auto app = ECSApplication(std::move(core));

    if (app.start()) {
        app.run();
    }
    
    app.end();

    return 0;
}
