#include "input.hpp"
#include "control.hpp"

#include <iostream>
#include <cassert>

int main()
{
    using namespace Machine;

    std::ifstream stream{"example_1_3.m"};
    machine_t machine(stream);
    std::ofstream ostream{"example_1_3_o.m"};
    machine.store(ostream);

    auto test = [&machine](const std::string &arg) -> void
    {
        machine.initialise(arg);
        machine.run();

        if (machine.state() == machine_t::machine_state_t::blocked)
            std::cout << "On input \"" << arg << "\" the machine blocks" << std::endl;
        else if (machine.state() == machine_t::machine_state_t::halted)
            std::cout << "On input \"" << arg << "\" the machine halts with output \"" << 
                machine.output()[0].to_ascii() << '\"' << std::endl;
        else
            assert(false and "An error has occured while running the machine");
        return;
    };
    
    test("a");
    test("bbababa");
    test("ababb");
    test("baababbab");

    return 0;
}

