#include "console.hpp"

#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>

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
    using namespace Machine;

    std::string program_name;
    std::string input;
    std::string output;
    bool empty_input = true;
    bool empty_output = true;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    bool pause_halt = true;
    bool tui = false;
    bool check_deterministic = false;
    bool deterministic;
    index_t time = negative_1;
    index_t n_threads = negative_1;

    for (int argi = 1; argi < argc;)
    {
        std::string s{argv[argi]};
        if (s == "--help")
        {
            std::cout <<
                "\tusage: run [args]"
                "\n\nArguments:"
                "\n\t--help\t\tDisplay this help message"
                "\n\t-p\t\tSpecify the program name"
                "\n\t-i\t\tInput for all devices"
                "\n\t-I\t\tA set of inputs, one for each device"
                "\n\t-o\t\tA single output to be searched in all devices output for a match"
                "\n\t-O\t\tA set of ouputs to be matched in every device"
                "\n\t-nph\t\tDo not pause the run_all command if a machine halts"
                "\n\t-tui\t\tLaunch the tui (i.e. run interactively)"
                "\n\t-d\t\t[Yes or No] assert that the program is or not deterministic"
                "\n\t-t\t\tSpecify the waiting time for the run (in seconds)"
                "\n\t-nt\t\tSpecify the number of threads used in the simulation"
                "\n\nNote:"
                "\n\tWhen specifying the input and output, an empty string is represented by '\\'."
                "\n\tTo specify any string starting with '-', or '\\' enter the string"
                "\n\tpreceded by a single additional '\\' character."
                "\n\nReturn value:"
                "\n\t0\t\tThe program run without errors"
                "\n\t1\t\tError reading the command"
                "\n\t2\t\tError reading the program"
                "\n\t3\t\tMismatch in the number of inputs and devices"
                "\n\t4\t\tMismatch in the number of outputs and devices"
                "\n\t5\t\tThe specified output does not match the obtained one"
                "\n\t6\t\tThe deterministic assertion failed"
                "\n\t7\t\tThe program did not halt in the specified time";

            std::cout << std::endl;

            return 0;
        }
        else if (s == "-p")
        {
            if (++argi >= argc)
                return 1;
            program_name = argv[argi++];
        }
        else if (s == "-i")
        {
            if (++argi >= argc)
                return 1;
            input = argv[argi++];
            if (std::empty(input) or input.front() == '-')
                return 1;
            if (input.front() == '\\')
                input = std::string{++std::cbegin(input), std::cend(input)};
            empty_input = false;
        }
        else if (s == "-I")
        {
            for (++argi; argi < argc; ++argi)
            {
                std::string s{argv[argi]};
                if (std::empty(s))
                    return 1;
                if (s.front() == '-')
                    break;
                if (s.front() == '\\')
                    s = std::string{++std::cbegin(s), std::cend(s)};
                inputs.emplace_back(s);
            }

            if (std::empty(inputs))
                return 1;
        }
        else if (s == "-o")
        {
            if (++argi >= argc)
                return 1;
            output = argv[argi++];
            if (std::empty(output) or output.front() == '-')
                return 1;
            if (output.front() == '\\')
                output = std::string{++std::cbegin(output), std::cend(output)};
            empty_output = false;
        }
        else if (s == "-O")
        {
            for (++argi; argi < argc; ++argi)
            {
                std::string s{argv[argi]};
                if (std::empty(s))
                    return 1;
                if (s.front() == '-')
                    break;
                if (s.front() == '\\')
                    s = std::string{++std::cbegin(s), std::cend(s)};
                outputs.emplace_back(s);
            }

            if (std::empty(outputs))
                return 1;
        }
        else if (s == "-nph")
        {
            pause_halt = false;
            ++argi;
        }
        else if (s == "-tui")
        {
            tui = true;
            ++argi;
        }
        else if (s == "-d")
        {
            check_deterministic = true;
            if (++argi >= argc)
                return 1;
            std::string s{argv[argi++]};
            std::transform(std::cbegin(s), std::cend(s), std::begin(s),
                [](unsigned char c){ return std::tolower(c); });

            if (s == "yes")
                deterministic = true;
            else if (s == "no")
                deterministic = false;
            else
                return 1;
        }
        else if (s == "-t")
        {
            if (++argi >= argc)
                return 1;
            time = std::atoll(argv[argi++]);
            if (time == 0)
                return 1;
        }
        else if (s == "-nt")
        {
            if (++argi >= argc)
                return 1;
            n_threads = std::atoll(argv[argi++]);
            if (n_threads == 0)
                return 1;
        }
        else
            return 1;
    }

    if (n_threads == negative_1)
        n_threads = console_t::default_n_threads();

    console_t console{n_threads};

    if (std::empty(program_name) and not tui)
        return 1;

    if (not std::empty(program_name))
    {
        try { console.load_program(program_name); }
        catch (std::runtime_error &) { return 2; }
        if (console.state() == console_t::console_state_t::empty)
            return 2;
    }

    if (std::empty(program_name) and not (empty_input and std::empty(inputs)))
        return 1;

    if ((empty_input and std::empty(inputs) and not tui) or
        (not empty_input and not std::empty(inputs)))
        return 1;

    if (not empty_input)
        console.initialise(input);
    else if (not std::empty(inputs))
    {
        try { console.initialise_individually(inputs); }
        catch (std::runtime_error &) { return 3; }
    }

    console.pause_halt() = pause_halt;

    if (check_deterministic and deterministic !=
            (console.state() & console_t::console_state_t::deterministic))
        return 6;
    
    if (not tui)
    {
        console.run_all();
        if (time == negative_1)
            console.wait();
        else if (not console.wait_for(std::chrono::seconds{time}))
            return 7;
    }

    else
        console.launch_tui();

    if (not empty_output and not console.find_output(output))
        return 5;

    if (not std::empty(outputs))
    {
        try
        {
            if (not console.find_output(outputs))
                return 5;
        }
        catch (std::runtime_error &) { return 4; }
    }

    return 0;
}

