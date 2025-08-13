#include "machine.hpp"

#include <iostream>
#include <sstream>

using namespace Machine;
    
void run(machine_t &machine, std::string input)
{
    while (std::size(input) != 0 and std::isspace(input.back()))
        input.pop_back();
            
    machine.initialise(input);
    machine.run();

    if (machine.state() == machine_t::machine_state_t::blocked)
        std::cout << "On input \"" << input << "\" the machine blocks" << std::endl;

    else if (machine.state() == machine_t::machine_state_t::halted)
    {
        std::cout << "On input \"" << input << "\" the machine halts with the following output:\n"; 
        for (index_t i = 0; i != std::size(machine.devices()); ++i)
        {
            if (std::empty(machine.output()[i]))
                continue;
            std::cout << "Device " << i << ":\t";
            std::cout << machine.output()[i] << '\n';
        }
    }

    else
        std::cerr << "An error has occured while running the machine with input\n" << input << '\n';

    return;
}

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage:\n\tEnter the file name containing the program as first input and " <<
            "the file name containing the program inputs for each round as next inputs or\n\t" <<
            "Enter the file containing the program as first input and the program input in" <<
            "std::cin\n.";
        return 1;
    }

    std::ifstream stream{argv[1]};
    if (not stream.is_open())
    {
        std::cerr << "Cannot open the file\n\t" << argv[1] << "\nAborting.\n";
        return 2;
    }

    
    machine_t machine(stream);
    stream.close();
    if (not machine.deterministic())
    {
        std::cerr << "Non-deterministic runs are still not implemented.\nAborting.\n";
        return 3;
    }

    if (argc == 2)
    {
        std::string input;
        while(std::cin >> input)
            run(machine, input);
    }

    else
        for (int i = 2; i != argc; ++i)
        {
            stream.open(argv[i]);
            if (not stream.is_open())
            {
                std::cerr << "Cannot open the file\n\t" << argv[i] << "\nAborting.\n";
                return 4;
            }

            stream.exceptions(std::ifstream::goodbit);
            std::string input;
            while (std::getline(stream, input) and not stream.eof())
                run(machine, input);
        }

    return 0;
}

