#include "input.hpp"
#include "control.hpp"

#include <iostream>
#include <cassert>

int main()
{
    using namespace Machine;

    /*const alphabet_t alphabet{3};
    std::vector<std::unique_ptr<device_t>> devices;

    std::unordered_map<index_t, string_t> terminating_states;
    terminating_states[6] = string_t{"Accept"};

    devices.emplace_back(new control_t{std::move(terminating_states)});
    devices.emplace_back(new input_t{alphabet});

    std::vector<std::shared_ptr<operation_t>> instruction_set;

    std::shared_ptr<operation_t> eof{new input_operation_scan_t{0}};
    std::shared_ptr<operation_t> a{new input_operation_scan_t{1}};
    std::shared_ptr<operation_t> b{new input_operation_scan_t{2}};

    instruction_set.emplace_back(new control_operation_t{0, 1});
    instruction_set.emplace_back(a);
    
    instruction_set.emplace_back(new control_operation_t{0, 0});
    instruction_set.emplace_back(b);
    
    instruction_set.emplace_back(new control_operation_t{1, 1});
    instruction_set.emplace_back(a);
    
    instruction_set.emplace_back(new control_operation_t{1, 2});
    instruction_set.emplace_back(b);
    
    instruction_set.emplace_back(new control_operation_t{2, 3});
    instruction_set.emplace_back(a);
    
    instruction_set.emplace_back(new control_operation_t{2, 0});
    instruction_set.emplace_back(b);
    
    instruction_set.emplace_back(new control_operation_t{3, 1});
    instruction_set.emplace_back(a);
    
    instruction_set.emplace_back(new control_operation_t{3, 4});
    instruction_set.emplace_back(b);
    
    instruction_set.emplace_back(new control_operation_t{4, 3});
    instruction_set.emplace_back(a);
    
    instruction_set.emplace_back(new control_operation_t{4, 5});
    instruction_set.emplace_back(b);
    
    instruction_set.emplace_back(new control_operation_t{5, 5});
    instruction_set.emplace_back(a);
    
    instruction_set.emplace_back(new control_operation_t{5, 5});
    instruction_set.emplace_back(b);
    
    instruction_set.emplace_back(new control_operation_t{5, 6});
    instruction_set.emplace_back(eof);

    machine_t machine{std::move(devices), std::move(instruction_set)};*/
    std::ifstream stream{"example_1_3.m"};
    machine_t machine(stream);
    std::ofstream ostream{"example_1_3_o.m"};
    machine.store(ostream);

    auto test = [&machine](const std::string &arg) -> void
    {
        string_t string{machine.alphabet()};
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

