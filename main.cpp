#include "game.h"

int main() {
    if (!initialize_allegro()) {
        return -1;
    }

    if (!initialize_game()) {
        cleanup_allegro();
        return -1;
    }

    Alma();//run_game

    cleanup_game();
    cleanup_allegro();

    return 0;
}