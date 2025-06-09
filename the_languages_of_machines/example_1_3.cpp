#include "input.hpp"
#include "control.hpp"

#include <iostream>
#include <cassert>

int main()
{
    using namespace Machine;

    const alphabet_t alphabet{3};
    std::vector<std::unique_ptr<device_t>> devices;

    std::unique_ptr<control_terminator_t> terminator {new
        control_terminator_recogniser_t{std::array<index_t, 1>{6}, std::array<index_t, 0>{}}};

    devices.emplace_back(new control_t{7, std::move(terminator)});
    devices.emplace_back(new input_t{alphabet});

    std::vector<std::unique_ptr<operation_t>> instruction_set;

    character_t eof{0}, a{1}, b{2}; 
    instruction_set.emplace_back(new control_operation_t{0, 1});
    instruction_set.emplace_back(new input_scan_operation_t{a});
    
    instruction_set.emplace_back(new control_operation_t{0, 0});
    instruction_set.emplace_back(new input_scan_operation_t{b});
    
    instruction_set.emplace_back(new control_operation_t{1, 1});
    instruction_set.emplace_back(new input_scan_operation_t{a});
    
    instruction_set.emplace_back(new control_operation_t{1, 2});
    instruction_set.emplace_back(new input_scan_operation_t{b});
    
    instruction_set.emplace_back(new control_operation_t{2, 3});
    instruction_set.emplace_back(new input_scan_operation_t{a});
    
    instruction_set.emplace_back(new control_operation_t{2, 0});
    instruction_set.emplace_back(new input_scan_operation_t{b});
    
    instruction_set.emplace_back(new control_operation_t{3, 1});
    instruction_set.emplace_back(new input_scan_operation_t{a});
    
    instruction_set.emplace_back(new control_operation_t{3, 4});
    instruction_set.emplace_back(new input_scan_operation_t{b});
    
    instruction_set.emplace_back(new control_operation_t{4, 3});
    instruction_set.emplace_back(new input_scan_operation_t{a});
    
    instruction_set.emplace_back(new control_operation_t{4, 5});
    instruction_set.emplace_back(new input_scan_operation_t{b});
    
    instruction_set.emplace_back(new control_operation_t{5, 5});
    instruction_set.emplace_back(new input_scan_operation_t{a});
    
    instruction_set.emplace_back(new control_operation_t{5, 5});
    instruction_set.emplace_back(new input_scan_operation_t{b});
    
    instruction_set.emplace_back(new control_operation_t{5, 6});
    instruction_set.emplace_back(new input_scan_operation_t{eof});

    machine_t machine{std::move(devices), std::move(instruction_set)};

    if constexpr (true)
    {
        auto machine_instruction_set = machine.instruction_set();
        for (auto i = std::cbegin(machine_instruction_set); i != std::cend(machine_instruction_set);
                i += 2)
        {
            std::cout << "From = " << dynamic_cast<const control_operation_t &>(**i).from();
            std::cout << ";\tTo = " << dynamic_cast<const control_operation_t &>(**i).to();
            std::cout << std::endl;
        }
    }

    auto test = [&machine, &alphabet](const std::string &arg) -> void
    {
        string_t string{alphabet};
        string.push(0);
        for (auto i = std::crbegin(arg); i != std::crend(arg); ++i)
        {
            if (*i == 'a')
                string.push(1);
            else if (*i == 'b')
                string.push(2);
            else
                assert(false and "Wrong character in std::string");
        }

        machine.initialise(string);
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
