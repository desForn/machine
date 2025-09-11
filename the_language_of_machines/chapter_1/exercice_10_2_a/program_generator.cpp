#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

int main(int argc, const char **argv)
{
    int p, q;

    {
        bool right_input = argc == 3;

        if (right_input)
        {
            p = std::atoi(argv[1]);
            q = std::atoi(argv[2]);

            if (p < 0 or q < 0)
                right_input = false;
        }

        if (not right_input)
        {
            std::cout << "This program outputs a program for an acceptor of the language"
                " { a^(np)b^(nq) | n ≥ 0 }\nInvoke the program as program_generator p q.\n";
            return 1;
        }
    }

    std::ofstream program{"program"}, test_accept{"test_accept"}, test_block{"test_block"};

    if (not p and not q)
        program << "control 0 : Accept 0;\ninput alphabetic 2;\n\n";
    else if (not p and q)
    {
        program << "control 0 : Accept 0;\ninput alphabetic 2;\n\n";

        if (q == 1)
            program << "0 to 0;\t\tscan b;\n\n";

        else
        {
            for (int i = 0; i != q - 1; ++i)
                program << i << " to " << i + 1 << ";\t\tscan b;\n\n";

            program << q - 1 << " to 0;\t\tscan b;\n\n";
        }
    }
    else if (p and not q)
    {
        program << "control 0 : Accept 0;\ninput alphabetic 2;\n\n";

        if (p == 1)
            program << "0 to 0;\t\tscan a;\n\n";

        else
        {
            for (int i = 0; i != p - 1; ++i)
                program << i << " to " << i + 1 << ";\t\tscan a;\n\n";

            program << p - 1 << " to 0;\t\tscan a;\n\n";
        }
    }
    else
    {
        program << "control 0 : Accept 0 " << p;
        program << ";\ninput alphabetic 2;\nucounter zero zero;\n\n";

        program << "0 to " << (q == 1 ? p : p + 1) << ";\t\tscan b;\t\t";
        program << (q == 1 ? "dec" : "noop") << ";\n";

        for (int i = 0; i != p - 1; ++i)
            program << i << " to " << i + 1 << ";\t\tscan a;\t\tnoop;\n";

        program << p - 1 << " to 0;\t\tscan a;\t\tinc;\n";
        for (int i = 0; i != q - 1; ++i)
            program << i + p << " to " << i + p + 1 << ";\t\tscan b;\t\tnoop;\n";

        program << p + q - 1 << " to " << p << ";\t\tscan b;\t\tdec;\n";
    }

    std::random_device rd{};
    std::mt19937 gen{rd()};

    for (int n = 0; n != 10; ++n)
    {
        std::string test;

        for (int i = 0; i != n * p; ++i)
            test += 'a';

        for (int i = 0; i != n * q; ++i)
            test += 'b';
                 
        test_accept << (std::empty(test) ? "\\" : test) << '\n';

        std::ranges::shuffle(test, gen);

        if (std::empty(test))
            continue;

        bool identity_permutation = std::equal(std::cbegin(test), std::cend(test) - 1,
                std::cbegin(test) + 1, [](char a, char b) { return not (a == 'b' and b == 'a'); });

        if (not identity_permutation)
            test_block << test << '\n';
    }

    for (int n = 0; n != 10; ++n)
    {
        std::string test;

        int ja = gen() % 5;
        int jb = gen() % 5;

        if (ja * q == jb * p and not (q == 0 and p == 0 and (ja != 0 or jb != 0)))
            continue;

        for (int i = 0; i != n * p + ja; ++i)
            test += 'a';

        for (int i = 0; i != n * q + jb; ++i)
            test += 'b';

        test_block << (std::empty(test) ? "\\" : test) << '\n';
    }

    return 0;
}

