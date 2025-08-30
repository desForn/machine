#include "console.hpp"

int main()
{
    Machine::console_t console{};
    //console.load_program("the_language_of_machines/chapter_1/exercice_1_6_1/program_c_3ucm.m");
    console.load_program("m");
    console.initialise("");

    //for (int i = 0; i != 25; ++i)
        //console.step_all();
    console.launch_tui();

    return 0;
}

