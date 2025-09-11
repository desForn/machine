#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

int main(int argc, const char **argv)
{
    int alpha, beta, gamma, delta;

    {
        bool right_input = (argc == 5);

        if (right_input)
        {
            alpha = std::atoi(argv[1]);
            beta = std::atoi(argv[2]);
            gamma = std::atoi(argv[3]);
            delta = std::atoi(argv[4]);

            if (alpha < 0 or alpha >= gamma or beta < 0 or beta >= delta or gamma < 0 or delta < 0)
                right_input = false;
        }

        if (not right_input)
        {
            std::cout << "This program outputs a program for a recognizer of the language"
                " { w ∈ {a,b}* | #(a)(w) = α (mod γ) ∧ #(b)(w) = β (mod δ) }\nInvoke the program as"
                "program_generator α β γ δ.\n";
            return 1;
        }
    }

    std::ofstream program{"program"}, test_accept{"test_accept"}, test_reject{"test_reject"};

    program << "control 0 : Accept " << alpha + beta * gamma << " : Reject";

    for (int i = 0; i != gamma * delta; ++i)
        if (i != alpha + beta * gamma)
            program << ' ' << i;

    program << ";\ninput alphabetic 2;\n\n";
    
    for (int i = 0; i != gamma * delta; ++i)
    {
        int ia = i % gamma;
        int ib = i / gamma;

        int ja = (ia == gamma - 1) ? 0 : ia + 1;
        int jb = (ib == delta - 1) ? 0 : ib + 1;

        program << i << " to " << ja + ib * gamma << ";\t\tscan a;\n";
        program << i << " to " << ia + jb * gamma << ";\t\tscan b;\n\n";
    }

    std::random_device rd{};
    std::mt19937 gen{rd()};

    for (int i = 0; i != 10; ++i)
        for (int ia = 0; ia <= i; ++ia)
        {
            int ib = i - ia;
            std::string test;

            for (int a = 0; a != alpha + ia * gamma; ++a)
                test += 'a';

            for (int b = 0; b != beta + ib * delta; ++b)
                test += 'b';

            std::ranges::shuffle(test, gen);
            test_accept << (std::empty(test) ? "\\" : test) << '\n';
        }

    for (int i = 0; i != 10; ++i)
        for (int ia = 0; ia <= i; ++ia)
        {
            int ib = i - ia;
            std::string test;

            int ja = gen() % gamma;
            int jb = gen() % delta;

            if (ja == 0 and jb == 0)
                continue;

            int na = alpha + ia * gamma - ja;
            int nb = beta + ib * delta - jb;

            if (na < 0 or nb < 0)
                continue;

            for (int a = 0; a != na; ++a)
                test += 'a';

            for (int b = 0; b != nb; ++b)
                test += 'b';

            std::ranges::shuffle(test, gen);
            test_reject << (std::empty(test) ? "\\" : test) << '\n';
        }

    return 0;
}

