#include "mud_common.h"

#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    SetConsoleOutputCP(CP_UTF8);

    MudGame game;
    game.run();
    return 0;
}
