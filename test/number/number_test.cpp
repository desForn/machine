#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <string>

#include "string.hpp"
#include "encoder.hpp"
#include "number.hpp"

using namespace Machine;

int main()
{
    std::ifstream data{"number_test.dat"};
    data.exceptions(std::ios_base::eofbit);
    std::string line;
    int line_counter = 0;
    int error = 0;

    auto get_line = [&]() -> bool
    {
        bool ret = true;
        std::getline(data, line);
        ++line_counter;

        for (char c = std::empty(line) ? 0 : line.front();
             std::empty(line) or not ((c >= '0' and c <= '9') or (c >= 'a' and c <= 'z') or
                    c == '+' or c == '-');
             c = std::empty(line) ? 0 : line.front())
        {
            std::getline(data, line);
            ++line_counter;
            ret = false;
        }

        return ret;
    };

    auto check = [&](const auto &arg0, const auto &arg1) -> void
    {
        if (arg0 != arg1)
        {
            error = 1;
            std::cerr << "Failed test in line " << line_counter;

            if constexpr (std::same_as<std::remove_cvref_t<decltype(arg0)>, std::string>)
                std::cerr << "; arg0 = \"" << arg0 << "\"";
            if constexpr (std::same_as<std::remove_cvref_t<decltype(arg1)>, std::string>)
                std::cerr << "; arg1 = \"" << arg1 << "\"";
            if constexpr (std::same_as<std::remove_cvref_t<decltype(arg0)>, signed_number_t> or
                          std::same_as<std::remove_cvref_t<decltype(arg0)>, unsigned_number_t>)
                std::cerr << "; arg0 = " << arg0.print_b_ary(16);
            if constexpr (std::same_as<std::remove_cvref_t<decltype(arg1)>, signed_number_t> or
                          std::same_as<std::remove_cvref_t<decltype(arg1)>, unsigned_number_t>)
                std::cerr << "; arg1 = " << arg1.print_b_ary(16);
            std::cerr << std::endl;
        }

        return;
    };

    try
    {
        get_line();

        do
            check(unsigned_number_t{static_cast<index_t>(std::stoul(line))}.print_b_ary(10), line);
        while (get_line());
        do
            check(unsigned_number_t{line, encoder_b_ary_t{10}}.print_b_ary(10), line);
        while (get_line());

        do
            check(unsigned_number_t{line, encoder_b_ary_t{16}}.print_b_ary(16), line);
        while (get_line());

        do
            check(unsigned_number_t{line, encoder_b_adic_t{9}}.print_b_adic(9), line);
        while (get_line());
        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t b{line, encoder_b_ary_t{10}};
            check (a > b, true);
            check (a < b, false);
            check (a >= b, true);
            check (a <= b, false);
            check (a == b, false);
            check (a != b, true);
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t b{line, encoder_b_ary_t{10}};
            check (a > b, false);
            check (a < b, false);
            check (a >= b, true);
            check (a <= b, true);
            check (a == b, true);
            check (a != b, false);
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{16}};
            unsigned_number_t b{a};
            ++a;
            ++a;
            ++a;
            check(a, b + 3);
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{16}};
            unsigned_number_t b{a};
            --a;
            --a;
            --a;
            check(a, b - 3);
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t b{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t c{line, encoder_b_ary_t{10}};
            check(a + b, c);
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t b{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t c{line, encoder_b_ary_t{10}};
            check(a - b, c);
        }
        while (get_line());

        do
        {
            unsigned_number_t o{line, encoder_b_ary_t{10}};
            unsigned_number_t a{o};
            unsigned_number_t b{o};
            try { get_line(); }
            catch (...) { return 2; }
            index_t c = static_cast<index_t>(std::stoll(line));
            for (index_t i = 0; i != c; ++i)
            {
                a = a << 1;
                b *= 2;
                check(a, b);
            }
            check(o << c, b);
        }
        while (get_line());

        do
        {
            unsigned_number_t o{line, encoder_b_ary_t{10}};
            unsigned_number_t a{o};
            unsigned_number_t b{o};
            try { get_line(); }
            catch (...) { return 2; }
            index_t c = static_cast<index_t>(std::stoll(line));
            for (index_t i = 0; i != c; ++i)
            {
                a = a >> 1;
                b /= 2;
                check(a, b);
            }

            check(o >> c, b); 
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t b{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t c{line, encoder_b_ary_t{10}};
            check(a * b, c);
        }
        while (get_line());

        do
        {
            unsigned_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t b{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t c{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            unsigned_number_t d{line, encoder_b_ary_t{10}};
            check(a / b, c);
            check(a % b, d);
        }
        while (get_line());

        do
        {
            signed_number_t a{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            signed_number_t b{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            signed_number_t c{line, encoder_b_ary_t{10}};
            try { get_line(); }
            catch(...) { return 2; }
            signed_number_t d{line, encoder_b_ary_t{10}};
            check(a / b, c);
            check(a % b, d);
        }
        while (get_line());
    }
    catch (std::ios_base::failure &) {}

    return error;
}
