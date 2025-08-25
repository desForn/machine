#include "console.hpp"

int main()
{
    Machine::console_t console{};
    *console.program_name() = "the_language_of_machines/chapter_1/exercice_1_6_1/program_c_3ucm.m";
    console.load_program();

    console.tui_loop();

    return 0;
}

