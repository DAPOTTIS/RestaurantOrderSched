#include "GUI.h"
#include "CLI.h"

#include <thread>

int main() {
    //if you want anything in the terminal, add it to the CLI class
    CLI cli;
    std::thread cli_thread(&CLI::init, &cli);

    GUI gui;
    gui.main_loop();
    cli_thread.join();
}