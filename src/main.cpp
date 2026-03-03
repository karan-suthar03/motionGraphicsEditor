#include "app_window.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand((unsigned int)time(nullptr));
    AppWindow::Run();
    return 0;
}