#include "machine.hpp"
#include "hash.hpp"
#include "control.hpp"
#include "input.hpp"
#include "output.hpp"
#include "stack.hpp"
#include "queue.hpp"
#include "unsigned_counter.hpp"
#include "counter.hpp"
#include "tape.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <cctype>
#include <map>
#include <unordered_map>
#include <utility>
#include <string>
#include <sstream>
#include <limits>

namespace Machine
{
    std::shared_ptr<operation_t> control_operation(character_t from, character_t to)
    {
        static std::unordered_map<std::array<character_t, 2>, std::shared_ptr<operation_t>> map;

        const std::array<character_t, 2> arr{from, to};
        auto it = map.find(arr);

        if (it != std::end(map))
            return it->second;

        auto a = map.insert(std::pair<std::array<character_t, 2>, std::shared_ptr<operation_t>>
                {arr, std::make_shared<control_operation_t>(from, to)});

        if (not a.second)
            throw std::runtime_error{"In Machine::control_operation(character_t, character_t):\n"
                "Insertion to std::unordered_map failed.\n"};

        return a.first->second;
    }

    std::shared_ptr<operation_t> operation(character_t operation_id, character_t operation_argument)
    {
        static const std::runtime_error error_0{
            "In Machine::operation(const std::array<character_t, 2> &):\n"
                "Operation_argument must be 0.\n"};
        static const std::runtime_error error_i{
            "In Machine::operation(const std::array<character_t, 2> &):\n"
                "Insertion to std::unordered_map failed.\n"};
        static std::unordered_map<std::array<character_t, 2>, std::shared_ptr<operation_t>> map;

        const std::array<character_t, 2> arg{operation_id, operation_argument};
        auto it = map.find(arg);

        if (it != std::end(map))
            return it->second;

        switch (arg[0])
        {
            case 7:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg, std::make_shared<noop_operation_t>()});

                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 8:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<input_operation_scan_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 9:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<input_operation_next_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 10:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<input_operation_eof_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 11:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<output_operation_write_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 12:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<stack_operation_push_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 13:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<stack_operation_pop_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 14:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<stack_operation_top_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 15:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<stack_operation_empty_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 16:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<unsigned_counter_operation_inc_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 17:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<unsigned_counter_operation_dec_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 18:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<unsigned_counter_operation_zero_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 19:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<unsigned_counter_operation_non_zero_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 20:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<counter_operation_inc_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 21:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<counter_operation_dec_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 22:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<counter_operation_zero_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 23:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<counter_operation_pos_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 24:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<counter_operation_neg_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 25:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<tape_operation_see_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 26:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<tape_operation_print_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 27:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<tape_operation_move_l_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 28:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<tape_operation_move_r_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 29:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<tape_operation_athome_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 30:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<queue_operation_enqueue_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 31:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<queue_operation_dequeue_t>(static_cast<character_t>(arg[1]))});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            case 32:
            {
                if (arg[1] != 0)
                    throw error_0;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                    std::shared_ptr<operation_t>>{arg,
                    std::make_shared<queue_operation_empty_t>()});
                if (not a.second)
                    throw error_i;
                return a.first->second;
            }

            default:
                throw error_i;
        }
    }

    std::shared_ptr<operation_t> compound_tape_operation(character_t flag, 
            character_t see_character, character_t print_character)
    {
        static const std::string error{
            "In Machine::compound_tape_operation(character_t, character_t, character_t).\n"};
        static std::unordered_map<std::array<character_t, 3>, std::shared_ptr<operation_t>> map;

        const std::array<character_t, 3> arg{flag, see_character, print_character};
        auto it = map.find(arg);

        if (it != std::end(map))
            return it->second;

        std::array<std::shared_ptr<operation_t>, 4> operations_{};
        if (arg[0] & 1)
            operations_[0] = operation(29, 0);

        if (arg[0] & 2)
            operations_[1] = operation(25, arg[1]);
        else if (arg[1] != 0)
            throw std::runtime_error{error + "Flag is marked as without see operation but "
                "see_character != 0.\n"};

        if (arg[0] & 4)
            operations_[2] = operation(26, arg[2]);
        else if (arg[2] != 0)
            throw std::runtime_error{error + "Flag is marked as without print operation but "
                "print_character != 0.\n"};

        if (arg[0] & 8 and arg[0] & 16)
            throw std::runtime_error{error + "Flag is marked with move_l and move_r operation.\n"};

        if (arg[0] & 8)
            operations_[3] = operation(27, 0);

        if (arg[0] & 16)
            operations_[3] = operation(28, 0);

        auto a = map.insert(std::pair<std::array<character_t, 3>, std::shared_ptr<operation_t>>{
                arg, std::make_shared<tape_operation_compound_t>(operations_)});
        if (not a.second)
            throw std::runtime_error{error + "Insertion in hash map failed.\n"};
        return a.first->second;
    }

    void get_char(char &c, std::istream &stream, index_t &line, index_t &column)
    {
        stream.get(c);
        ++column;
        if (c == '\n' or c == '\r')
        {
            column = 1;
            ++line;
        }
    }

    std::string read_token(std::istream &stream, index_t &line, index_t &column)
    {
        std::string ret{};

        char a;
        get_char(a, stream, line, column);
        while (std::isspace(a))
            get_char(a, stream, line, column);
         
        while (a == '/')
        {
            while (a < 10 or a > 13)
                get_char(a, stream, line, column);
            while (std::isspace(a))
                get_char(a, stream, line, column);
        }
        ret += a;

        if (std::ispunct(a))
            return ret;

        index_t initial_line = line, initial_column = column;
        get_char(a, stream, line, column);

        while(a and not std::ispunct(a) and not std::isspace(a))
        {
            ret += a;
            initial_line = line;
            initial_column = column;
            get_char(a, stream, line, column);
        }
        
        line = initial_line;
        column = initial_column;
        stream.unget();

        return ret;
    }

    character_t read_character(std::istream &stream, index_t &line, index_t& column,
            const encoder_t &encoder)
    {
        auto initial_pos = stream.tellg();
        index_t initial_line = line, initial_column = column;

        std::string token;
        try
        {
            token = read_token(stream, line, column);
        }
        catch (...)
        {
            stream.seekg(initial_pos);
            line = initial_line;
            column = initial_column;
            throw;
        }

        if (std::size(token) != 1)
        {
            stream.seekg(initial_pos);
            line = initial_line;
            column = initial_column;
            throw std::runtime_error{"In Machine::read_character(std::istream &):\n"
                "Reading input at line " + std::to_string(line) + ":" + std::to_string(column) +
                "\nExpected single character, received string '" + token + "'.\n"};
        }

        character_t ret;

        try
        {
            ret = encoder(token[0]);
        }
        catch (...)
        {
            stream.seekg(initial_pos);
            line = initial_line;
            column = initial_column;
            throw;
        }

        return ret;
    }


    bool is_number(const std::string &arg)
        { return std::ranges::all_of(arg, [](char a){ return a >= '0' and a <= '9'; }); }

    std::unique_ptr<encoder_t> read_encoder(std::istream &stream, index_t &line, index_t &column)
    {
        static const std::string error{"In Machine::read_encoder(std::istream &):\n"};

        std::unique_ptr<encoder_t> ret;
        index_t initial_line = line, initial_column = column;
        auto initial_pos = stream.tellg();
        std::string token = read_token(stream, line, column);

        if (token == "alphabetic")
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            if (not is_number(token))
                throw std::runtime_error{error +
                    "Reading input at line " + std::to_string(initial_line) + ":" +
                        std::to_string(initial_column) + "\nExpected a number.\n"};
            character_t c = static_cast<character_t>(std::stoll(token));

            ret = std::make_unique<encoder_alphabetic_t>(c);
        }

        else if (token == "list")
        {
            std::vector<char> list;

            for (initial_line = line, initial_column = column,
                    token = read_token(stream, line, column);
                token != ":";
                initial_line = line, initial_column = column,
                     token = read_token(stream, line, column))
            {
                if (std::size(token) != 1)
                    throw std::runtime_error{error +
                    "Reading input at line " + std::to_string(initial_line) + ":" +
                        std::to_string(initial_column) + "\nExpected single character.\n"};
                list.emplace_back(token[0]);
            }

            ret = std::make_unique<encoder_list_t>(std::move(list));
        }

        else if (token == "ascii")
            ret = std::make_unique<encoder_ascii_t>();

        else if (token == "signed_b_ary" or token == "numeric")
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            if (not is_number(token))
                throw std::runtime_error{error +
                    "Reading input at line" + std::to_string(initial_line) + ":" +
                        std::to_string(initial_column) + "\nExpected a number.\n"};
            character_t c = static_cast<character_t>(std::stoll(token));

            ret = std::make_unique<encoder_signed_b_ary_t>(c);
        }

        else if (token == "signed_b_adic")
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            if (not is_number(token))
                throw std::runtime_error{error +
                    "Reading input at line" + std::to_string(initial_line) + ":" +
                        std::to_string(initial_column) + "\nExpected a number.\n"};
            character_t c = static_cast<character_t>(std::stoll(token));

            ret = std::make_unique<encoder_signed_b_adic_t>(c);
        }

        else if (token == "b_ary")
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            if (not is_number(token))
                throw std::runtime_error{error +
                    "Reading input at line" + std::to_string(initial_line) + ":" +
                        std::to_string(initial_column) + "\nExpected a number.\n"};
            character_t c = static_cast<character_t>(std::stoll(token));

            ret = std::make_unique<encoder_b_ary_t>(c);
        }

        else if (token == "b_adic")
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            if (not is_number(token))
                throw std::runtime_error{error +
                    "Reading input at line" + std::to_string(initial_line) + ":" +
                        std::to_string(initial_column) + "\nExpected a number.\n"};
            character_t c = static_cast<character_t>(std::stoll(token));

            ret = std::make_unique<encoder_b_adic_t>(c);
        }

        else
        {
            stream.seekg(initial_pos);
            line = initial_line;
            column = initial_column;
        }

       return ret; 
    }

    std::string print_encoder(const encoder_t &encoder)
    {
        std::string ret;
        if (typeid(encoder) == typeid(encoder_alphabetic_t))
        {
            ret = "alphabetic ";
            ret += std::to_string(encoder.alphabet().max_character());
        }

        else if (typeid(encoder) == typeid(encoder_list_t))
        {
            ret = "list ";

            for (const char i : dynamic_cast<const encoder_list_t &>(encoder).list())
            {
                ret += i;
                ret += ' ';
            }

            ret.pop_back();
        }

        else if (typeid(encoder) == typeid(encoder_ascii_t))
            ret = "ascii";

        else if (typeid(encoder) == typeid(encoder_signed_b_ary_t))
        {
            ret = "signed_b_ary ";
            ret += std::to_string(encoder.alphabet().max_character() + 1);
        }

        else if (typeid(encoder) == typeid(encoder_signed_b_adic_t))
        {
            ret = "signed_b_adic ";
            ret += std::to_string(encoder.alphabet().max_character() - 1);
        }

        else if (typeid(encoder) == typeid(encoder_b_ary_t))
        {
            ret = "b_ary ";
            ret += std::to_string(encoder.alphabet().max_character() + 1);
        }

        else if (typeid(encoder) == typeid(encoder_b_adic_t))
        {
            ret = "b_adic ";
            ret += std::to_string(encoder.alphabet().max_character() - 1);
        }

        else
            throw std::runtime_error{"In Machine::print_encoder(const encoder_t &):\n"
                "Unknown encoder class.\n"};

        return ret;
    }

    std::unique_ptr<device_t> read_device(std::istream &stream, index_t &line, index_t &column)
    {
        static bool initialised = false;
        static std::unordered_map<std::string, index_t> keywords;
        if (not initialised)
        {
            keywords["control"] = 0;
            keywords["input"] = 1;
            keywords["output"] = 2;
            keywords["stack"] = 3;
            keywords["ucounter"] = 4;
            keywords["counter"] = 5;
            keywords["tape"] = 6;
            keywords["queue"] = 7;
            
            initialised = true;
        }

        static const std::string error{
            "In read_device(std::istream &, const encoder_t &):\nInvalid input.\n"};

        const auto initial_pos = stream.tellg();
        std::string token;
        index_t initial_line, initial_column;

        auto get_token = [&]() -> void
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            return;
        };

        auto get_encoder = [&]() -> std::unique_ptr<encoder_t>
        {
            initial_line = line;
            initial_column = column;
            return read_encoder(stream, line, column);
        };

        auto throw_error = [&](const std::string &arg) -> void
        {
            stream.seekg(initial_pos);
            throw std::runtime_error{"In Machine::read_device(std::istream &, index_t &, index_t &,"
                " const encoder_t &):\nReading input at line " + std::to_string(initial_line) + ":"
                + std::to_string(initial_column) + ".\n" + arg + "\n"};
        };

        try { get_token(); }
        catch (std::ios_base::failure &) { return {nullptr}; }


        const auto it = keywords.find(token);
        if (it == std::end(keywords))
        {
            stream.seekg(initial_pos);
            return {nullptr};
        }

        std::unique_ptr<encoder_t> encoder = get_encoder();

        switch (it->second)
        {
            case 0:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_ascii_t>();

                get_token();
                if (not is_number(token))
                    throw_error("Expected a number (the initial state).");
                index_t initial_state = static_cast<index_t>(std::stoll(token));

                get_token();

                std::unordered_map<index_t, string_t> terminating_states;
                while (token != ";")
                {
                    if (token != ":")
                        throw_error("Expected ':'.");


                    string_t terminator_string = (*encoder)(read_token(stream, line, column));
                    get_token();
                    while (token != ":" and token != ";")
                    {
                        if (not is_number(token))
                            throw_error("Expected a number (the terminating state).");

                        index_t terminating_state = static_cast<index_t>(std::stoll(token));
                        terminating_states[terminating_state] = terminator_string;
                        get_token();
                    }
                }

                std::unique_ptr<control_initialiser_t> initialiser =
                    std::make_unique<control_initialiser_t>(initial_state);

                std::unique_ptr<control_terminator_t> terminator = 
                    std::make_unique<control_terminator_t>(std::move(terminating_states));

                return std::make_unique<control_t>(
                    std::move(encoder), std::move(initialiser), std::move(terminator));
            }

            case 1:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_ascii_t>();

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<input_t>(std::move(encoder));
            }

            case 2:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_ascii_t>();

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<output_t>(std::move(encoder));
            }

            case 3:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_ascii_t>();

                get_token();
                if (token == ";")
                    return std::make_unique<stack_t>(std::move(encoder),
                            std::make_unique<stack_initialiser_string_t>(),
                            std::make_unique<stack_terminator_string_t>());

                std::unique_ptr<stack_initialiser_t> initialiser;
                if (token == "empty")
                    initialiser = std::make_unique<stack_initialiser_empty_t>();
                else if (token == "string")
                    initialiser = std::make_unique<stack_initialiser_string_t>();
                else
                    throw_error("Expected the initialiser : 'empty' or 'string'.");

                std::unique_ptr<stack_terminator_t> terminator;
                get_token();
                if (token == "empty")
                    terminator = std::make_unique<stack_terminator_empty_t>();
                else if (token == "string")
                    terminator = std::make_unique<stack_terminator_string_t>();
                else
                    throw_error("Expected the terminator : 'empty' or 'string'.");

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<stack_t>(std::move(encoder),
                        std::move(initialiser), std::move(terminator));
            }

            case 4:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_b_ary_t>(10);

                get_token();
                if (token == ";")
                    return std::make_unique<unsigned_counter_t>(std::move(encoder),
                        std::make_unique<unsigned_counter_initialiser_string_t>(),
                        std::make_unique<unsigned_counter_terminator_string_t>());

                std::unique_ptr<unsigned_counter_initialiser_t> initialiser;
                if (token == "zero")
                    initialiser = std::make_unique<unsigned_counter_initialiser_zero_t>();
                else if (token == "string")
                    initialiser = std::make_unique<unsigned_counter_initialiser_string_t>();
                else
                    throw_error("Expected the initialiser : 'zero' or 'string'.");

                std::unique_ptr<unsigned_counter_terminator_t> terminator;
                get_token();
                if (token == "zero")
                    terminator = std::make_unique<unsigned_counter_terminator_zero_t>();
                else if (token == "string")
                    terminator = std::make_unique<unsigned_counter_terminator_string_t>();
                else
                    throw_error("Expected the terminator : 'zero' or 'string'.");

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<unsigned_counter_t>(std::move(encoder),
                        std::move(initialiser), std::move(terminator));
            }

            case 5:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_signed_b_ary_t>(10);

                std::unique_ptr<counter_initialiser_t> initialiser;
                get_token();
                if (token == "zero")
                    initialiser = std::make_unique<counter_initialiser_zero_t>();
                else if (token == "string")
                    initialiser = std::make_unique<counter_initialiser_string_t>();
                else
                    throw_error("Expected the initialiser : 'zero' or 'string'.");

                std::unique_ptr<counter_terminator_t> terminator;
                get_token();
                if (token == "zero")
                    terminator = std::make_unique<counter_terminator_zero_t>();
                else if (token == "string")
                    terminator = std::make_unique<counter_terminator_string_t>();
                else
                    throw_error("Expected the terminator : 'zero' or 'string'.");

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<counter_t>(std::move(encoder),
                        std::move(initialiser), std::move(terminator));
            }

            case 6:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_ascii_t>();

                character_t c;
                try { c = read_character(stream, line, column, *encoder); }
                catch (std::runtime_error &) { throw_error("Expected the default character."); }

                std::unique_ptr<tape_initialiser_t> initialiser;
                get_token();
                if (token == "empty")
                    initialiser = std::make_unique<tape_initialiser_empty_t>();
                else if (token == "string")
                    initialiser = std::make_unique<tape_initialiser_string_t>();
                else
                    throw_error("Expected the initialiser : 'empty' or 'string'.");

                std::unique_ptr<tape_terminator_t> terminator;
                get_token();
                if (token == "athome")
                    terminator = std::make_unique<tape_terminator_athome_t>();
                else if (token == "always")
                    terminator = std::make_unique<tape_terminator_always_t>();
                else
                    throw_error("Expected the terminator : 'athome' or 'always'.");

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<tape_t>(std::move(encoder), c,
                        std::move(initialiser), std::move(terminator));
            }

            case 7:
            {
                if (not encoder)
                    encoder = std::make_unique<encoder_ascii_t>();

                std::unique_ptr<queue_initialiser_t> initialiser;
                get_token();
                if (token == "empty")
                    initialiser = std::make_unique<queue_initialiser_empty_t>();
                else if (token == "string")
                    initialiser = std::make_unique<queue_initialiser_string_t>();
                else
                    throw_error("Expected the initialiser : 'empty' or 'string'.");

                std::unique_ptr<queue_terminator_t> terminator;
                get_token();
                if (token == "empty")
                    terminator = std::make_unique<queue_terminator_empty_t>();
                else if (token == "string")
                    terminator = std::make_unique<queue_terminator_string_t>();
                else
                    throw_error("Expected the terminator : 'empty' or 'string'.");

                get_token();
                if (token != ";")
                    throw_error("Expected ';'.");

                return std::make_unique<queue_t>(std::move(encoder),
                        std::move(initialiser), std::move(terminator));
            }
        }
        
        std::unreachable();
    }

    std::string print_device(const device_t &device)
    {
        auto throw_error = [&](const std::string &arg) -> void
        {
            throw std::runtime_error{"In Machine::print_device(const device_t &):\n" + arg + "\n"};
        };

        std::string ret;
        if (typeid(device) == typeid(control_t))
        {
            const control_t &control = dynamic_cast<const control_t &>(device);
            const auto &terminating_states = control.terminator().terminating_states();

            std::multimap<string_t, index_t> map;
            for (const auto &i : terminating_states)
                map.insert(std::pair<string_t, index_t>{i.second, i.first});

            const string_t *last_string = nullptr;

            ret += "control ";
            ret += print_encoder(device.encoder());
            ret += " ";
            ret += std::to_string(control.initialiser().initial_state());
            ret += " ";

            for (const auto &i : map)
            {
                bool new_string;
                if (not last_string)
                    new_string = true;
                else
                    new_string = (i.first != *last_string);
                last_string = &i.first;

                if (new_string)
                    ret += ": " + device.encoder()(*last_string) + ", ";
                else
                    ret += ", ";
                ret += std::to_string(i.second);
            }
        }

        else if (typeid(device) == typeid(input_t))
        {
            ret += "input ";
            ret += print_encoder(device.encoder());
        }

        else if (typeid(device) == typeid(output_t))
        {
            ret += "output ";
            ret += print_encoder(device.encoder());
        }

        else if (typeid(device) == typeid(stack_t))
        {
            const stack_t &stack = dynamic_cast<const stack_t &>(device);
            const stack_initialiser_t &initialiser = stack.initialiser();
            const stack_terminator_t &terminator = stack.terminator();

            ret += "stack ";
            ret += print_encoder(device.encoder());
            ret += " ";

            if (typeid(initialiser) == typeid(stack_initialiser_empty_t))
                ret += "empty ";
            else if (typeid(initialiser) == typeid(stack_initialiser_string_t))
                ret += "string ";
            else
                throw_error("Unknown stack initialiser class.");

            if (typeid(terminator) == typeid(stack_terminator_empty_t))
                ret += "empty";
            else if (typeid(terminator) == typeid(stack_terminator_string_t))
                ret += "string";
            else
                throw_error("Unknown stack terminator class.");
        }

        else if (typeid(device) == typeid(unsigned_counter_t))
        {
            const unsigned_counter_t &unsigned_counter =
                dynamic_cast<const unsigned_counter_t &>(device);
            const unsigned_counter_initialiser_t &initialiser = unsigned_counter.initialiser();
            const unsigned_counter_terminator_t &terminator = unsigned_counter.terminator();

            ret += "ucounter ";
            ret += print_encoder(device.encoder());
            ret += " ";

            if (typeid(initialiser) == typeid(unsigned_counter_initialiser_zero_t))
                ret += "zero ";
            else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_string_t))
                ret += "string ";
            else
                throw_error("Unknown ucounter initialiser class.");

            if (typeid(terminator) == typeid(unsigned_counter_terminator_zero_t))
                ret += "zero";
            else if (typeid(terminator) == typeid(unsigned_counter_terminator_string_t))
                ret += "string";
            else
                throw_error("Unknown ucounter terminator class.");
        }

        else if (typeid(device) == typeid(counter_t))
        {
            const counter_t &counter = dynamic_cast<const counter_t &>(device);
            const counter_initialiser_t &initialiser = counter.initialiser();
            const counter_terminator_t &terminator = counter.terminator();

            ret += "counter ";
            ret += print_encoder(device.encoder());
            ret += " ";

            if (typeid(initialiser) == typeid(counter_initialiser_zero_t))
                ret += "zero ";
            else if (typeid(initialiser) == typeid(counter_initialiser_string_t))
                ret += "string ";
            else
                throw_error("Unknown counter initialiser class.");

            if (typeid(terminator) == typeid(counter_terminator_zero_t))
                ret += "zero";
            else if (typeid(terminator) == typeid(counter_terminator_string_t))
                ret += "string";
            else
                throw_error("Unknown counter terminator class.");
        }

        else if (typeid(device) == typeid(tape_t))
        {
            const tape_t &tape = dynamic_cast<const tape_t &>(device);
            const tape_initialiser_t &initialiser = tape.initialiser();
            const tape_terminator_t &terminator = tape.terminator();

            ret += "tape ";
            ret += " ";
            ret += print_encoder(device.encoder());
            ret += " ";
            ret += std::to_string(tape.default_character());
            ret += " ";

            if (typeid(initialiser) == typeid(tape_initialiser_empty_t))
                ret += "empty ";
            else if (typeid(initialiser) == typeid(tape_initialiser_string_t))
                ret += "string ";
            else
                throw_error("Unknown tape initialiser class.");

            if (typeid(terminator) == typeid(tape_terminator_athome_t))
                ret += "athome";
            else if (typeid(terminator) == typeid(tape_terminator_always_t))
                ret += "always";
            else
                throw_error("Unknown tape terminator class.");
        }

        else if (typeid(device) == typeid(queue_t))
        {
            const queue_t &queue = dynamic_cast<const queue_t &>(device);
            const queue_initialiser_t &initialiser = queue.initialiser();
            const queue_terminator_t &terminator = queue.terminator();

            ret += "queue ";
            ret += print_encoder(device.encoder());
            ret += " ";

            if (typeid(initialiser) == typeid(queue_initialiser_empty_t))
                ret += "empty ";
            else if (typeid(initialiser) == typeid(queue_initialiser_string_t))
                ret += "string ";
            else
                throw_error("Unknown queue initialiser class.");

            if (typeid(terminator) == typeid(queue_terminator_empty_t))
                ret += "empty";
            else if (typeid(terminator) == typeid(queue_terminator_string_t))
                ret += "string";
            else
                throw_error("Unknown queue terminator class.");
        }

        else
            throw_error("Unknown device class.");

        return ret;
    }

    std::shared_ptr<operation_t> read_operation
        (std::istream &stream, index_t &line, index_t &column,
         const device_t &device)
    {
        static bool initialised = false;
        static std::unordered_map<std::string, index_t> keywords;
        if (not initialised)
        {
            keywords["noop"] = 7;
            keywords["scan"] = 8;
            keywords["next"] = 9;
            keywords["eof"] = 10;
            keywords["write"] = 11;
            keywords["push"] = 12;
            keywords["pop"] = 13;
            keywords["top"] = 14;
            keywords["empty"] = 15;
            keywords["nonzero"] = 19;
            keywords["inc"] = 20;
            keywords["dec"] = 21;
            keywords["zero"] = 22;
            keywords["pos"] = 23;
            keywords["neg"] = 24;
            keywords["see"] = 25;
            keywords["print"] = 26;
            keywords["movel"] = 27;
            keywords["mover"] = 28;
            keywords["athome"] = 29;
            keywords["enqueue"] = 30;
            keywords["dequeue"] = 31;
            keywords["pml"] = 33;
            keywords["pmr"] = 34;
            keywords["sml"] = 35;
            keywords["smr"] = 36;
            keywords["sp"] = 37;
            keywords["spml"] = 38;
            keywords["spmr"] = 39;
            keywords["aml"] = 40;
            keywords["amr"] = 41;
            keywords["ap"] = 42;
            keywords["apml"] = 40;
            keywords["apmr"] = 43;
            keywords["as"] = 44;
            keywords["asml"] = 40;
            keywords["asmr"] = 45;
            keywords["asp"] = 46;
            keywords["aspml"] = 40;
            keywords["aspmr"] = 47;

            initialised = true;
        }

        static const std::string error{"In read_operation"
            "(std::istream &, const device_t &, const encoder_t &):\nInvalid input.\n"};

        const auto initial_pos = stream.tellg();
        std::string token;
        index_t initial_line, initial_column;

        auto get_token = [&]() -> void
        {
            initial_line = line;
            initial_column = column;
            token = read_token(stream, line, column);
            return;
        };

        auto throw_error = [&](const std::string &arg) -> void
        {
            stream.seekg(initial_pos);
            throw std::runtime_error{"In Machine::read_operation(std::istream &, index_t &,"
                " index_t &, const device_t &):\nReading input at line " +
                std::to_string(initial_line) + ":" + std::to_string(initial_column) + ".\n" + arg +
                "\n"};
        };

        const encoder_t &encoder = device.encoder();

        try { get_token(); }
        catch (std::ios_base::failure &) { return {nullptr}; }

        if (is_number(token))
        {
            if (typeid(device) != typeid(control_t))
                throw_error("Incorrect device. Expected control.");
            
            character_t from = static_cast<character_t>(std::stoll(token));

            get_token();
            if (token != "to")
                throw_error("Expected keyword 'to'.");

            get_token();
            if (not is_number(token))
                throw_error("Expected a number.");
            character_t to = static_cast<character_t>(std::stoll(token));

            get_token();
            if (token != ";")
                throw_error("Expected ';'.");

            return control_operation(from, to);
        }

        else
        {
            auto it = keywords.find(token);
            if (it == std::end(keywords))
                throw_error("Unknown instruction.");

            switch (it->second)
            {
                case 7:
                {
                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(7, 0);
                }

                case 8:
                {
                    if (typeid(device) != typeid(input_t))
                        throw_error("Incorrect device. Expected input.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(8, c);
                }

                case 9:
                {
                    if (typeid(device) != typeid(input_t))
                        throw_error("Incorrect device. Expected input.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(9, c);
                }

                case 10:
                {
                    if (typeid(device) != typeid(input_t))
                        throw_error("Incorrect device. Expected input.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(10, 0);
                }

                case 11:
                {
                    if (typeid(device) != typeid(output_t))
                        throw_error("Incorrect device. Expected output.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(11, c);
                }

                case 12:
                {
                    if (typeid(device) != typeid(stack_t))
                        throw_error("Incorrect device. Expected stack.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(12, c);
                }

                case 13:
                {
                    if (typeid(device) != typeid(stack_t))
                        throw_error("Incorrect device. Expected stack.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(13, c);
                }

                case 14:
                {
                    if (typeid(device) != typeid(stack_t))
                        throw_error("Incorrect device. Expected stack.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(14, c);
                }

                case 15:
                {
                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    if (typeid(device) == typeid(stack_t))
                        return operation(15, 0);

                    if (typeid(device) == typeid(queue_t))
                        return operation(32, 0);

                    throw_error("Incorrect device. Expected stack or queue.");
                    return {nullptr};
                }

                case 19:
                {
                    if (typeid(device) != typeid(unsigned_counter_t))
                        throw_error("Incorrect device. Expected ucounter.");

                    return operation(19, 0);
                }

                case 20:
                {
                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    if (typeid(device) == typeid(counter_t))
                        return operation(20, 0);

                    if (typeid(device) == typeid(unsigned_counter_t))
                        return operation(16, 0);

                    throw_error("Incorrect device. Expected counter or ucounter.");
                    return {nullptr};
                }

                case 21:
                {
                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    if (typeid(device) == typeid(counter_t))
                        return operation(21, 0);

                    if (typeid(device) == typeid(unsigned_counter_t))
                        return operation(17, 0);

                    throw_error("Incorrect device. Expected counter or ucounter.");
                    return {nullptr};
                }

                case 22:
                {
                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    if (typeid(device) == typeid(counter_t))
                        return operation(22, 0);

                    if (typeid(device) == typeid(unsigned_counter_t))
                        return operation(18, 0);

                    throw_error("Incorrect device. Expected counter or ucounter.");
                    return {nullptr};
                }

                case 23:
                {
                    if (typeid(device) != typeid(counter_t))
                        throw_error("Incorrect device. Expected counter.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(23, 0);
                }

                case 24:
                {
                    if (typeid(device) != typeid(counter_t))
                        throw_error("Incorrect device. Expected counter.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(24, 0);
                }

                case 25:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(25, c);
                }

                case 26:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(26, c);
                }

                case 27:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(27, 0);
                }

                case 28:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(28, 0);
                }

                case 29:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(29, 0);
                }

                case 30:
                {
                    if (typeid(device) != typeid(queue_t))
                        throw_error("Incorrect device. Expected queue.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(30, c);
                }

                case 31:
                {
                    if (typeid(device) != typeid(queue_t))
                        throw_error("Incorrect device. Expected queue.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return operation(31, c);
                }

                case 33:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(12, 0, c);
                }

                case 34:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(20, 0, c);
                }

                case 35:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(10, c, 0);
                }

                case 36:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(18, c, 0);
                }

                case 37:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);
                    character_t d = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(6, c, d);
                }

                case 38:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);
                    character_t d = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(14, c, d);
                }

                case 39:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);
                    character_t d = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(22, c, d);
                }

                case 40:
                {
                    throw_error("Inconsistent compound instruction '" + token +
                        "'; athome and move_l present.");
                    return {nullptr};
                }

                case 41:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(17, 0, 0);
                }

                case 42:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(5, 0, c);
                }

                case 43:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(21, 0, c);
                }

                case 44:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(3, c, 0);
                }

                case 45:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(19, c, 0);
                }

                case 46:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);
                    character_t d = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(7, c, d);
                }

                case 47:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw_error("Incorrect device. Expected tape.");

                    character_t c = read_character(stream, line, column, encoder);
                    character_t d = read_character(stream, line, column, encoder);

                    get_token();
                    if (token != ";")
                        throw_error("Expected ';'.");

                    return compound_tape_operation(23, c, d);
                }

                default:
                {
                    throw_error("Unknown instruction.");
                    return {nullptr};
                }
            } 
        }
    }

    std::string print_operation(const operation_t &operation, const encoder_t &encoder,
            index_t *control_state = nullptr)
    {
        std::string ret;

        if (typeid(operation) == typeid(control_operation_t))
        {
            const control_operation_t &op =
                dynamic_cast<const control_operation_t &>(operation);

            if (control_state)
                *control_state = op.from();

            ret += std::to_string(op.from());
            ret += " to ";
            ret += std::to_string(op.to());
        }

        else if (typeid(operation) == typeid(noop_operation_t))
            ret += "noop";

        else if (typeid(operation) == typeid(input_operation_scan_t))
        {
            const input_operation_scan_t &op =
                dynamic_cast<const input_operation_scan_t &>(operation);
            ret += "scan ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(input_operation_next_t))
        {
            const input_operation_next_t &op =
                dynamic_cast<const input_operation_next_t &>(operation);
            ret += "next ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(input_operation_eof_t))
            ret += "eof";

        else if (typeid(operation) == typeid(output_operation_write_t))
        {
            const output_operation_write_t &op =
                dynamic_cast<const output_operation_write_t &>(operation);
            ret += "write ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(stack_operation_push_t))
        {
            const stack_operation_push_t &op =
                dynamic_cast<const stack_operation_push_t &>(operation);
            ret += "push ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(stack_operation_pop_t))
        {
            const stack_operation_pop_t &op =
                dynamic_cast<const stack_operation_pop_t &>(operation);
            ret += "pop ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(stack_operation_top_t))
        {
            const stack_operation_top_t &op =
                dynamic_cast<const stack_operation_top_t &>(operation);
            ret += "top ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(stack_operation_empty_t))
            ret += "empty";

        else if (typeid(operation) == typeid(unsigned_counter_operation_inc_t))
            ret += "uinc";

        else if (typeid(operation) == typeid(unsigned_counter_operation_dec_t))
            ret += "udec";

        else if (typeid(operation) == typeid(unsigned_counter_operation_zero_t))
            ret += "uzero";

        else if (typeid(operation) == typeid(unsigned_counter_operation_non_zero_t))
            ret += "unonzero";

        else if (typeid(operation) == typeid(counter_operation_inc_t))
            ret += "inc";

        else if (typeid(operation) == typeid(counter_operation_dec_t))
            ret += "dec";

        else if (typeid(operation) == typeid(counter_operation_zero_t))
            ret += "zero";

        else if (typeid(operation) == typeid(counter_operation_pos_t))
            ret += "pos";

        else if (typeid(operation) == typeid(counter_operation_neg_t))
            ret += "neg";

        else if (typeid(operation) == typeid(tape_operation_see_t))
        {
            const tape_operation_see_t &op =
                dynamic_cast<const tape_operation_see_t &>(operation);
            ret += "see ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(tape_operation_print_t))
        {
            const tape_operation_print_t &op =
                dynamic_cast<const tape_operation_print_t &>(operation);
            ret += "print ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(tape_operation_move_l_t))
            ret += "movel";

        else if (typeid(operation) == typeid(tape_operation_move_r_t))
            ret += "mover";

        else if (typeid(operation) == typeid(tape_operation_athome_t))
            ret += "athome";

        else if (typeid(operation) == typeid(queue_operation_enqueue_t))
        {
            const queue_operation_enqueue_t &op =
                dynamic_cast<const queue_operation_enqueue_t &>(operation);
            ret += "enqueue ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(queue_operation_dequeue_t))
        {
            const queue_operation_dequeue_t &op =
                dynamic_cast<const queue_operation_dequeue_t &>(operation);
            ret += "dequeue ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(queue_operation_empty_t))
            ret += "empty";

        else if (typeid(operation) == typeid(queue_operation_enqueue_t))
        {
            const queue_operation_enqueue_t &op =
                dynamic_cast<const queue_operation_enqueue_t &>(operation);
            ret += "enqueue ";
            ret += encoder(op.character());
        }

        else if (typeid(operation) == typeid(tape_operation_compound_t))
        {
            const tape_operation_compound_t &op =
                dynamic_cast<const tape_operation_compound_t &>(operation);

            if (op.operations()[0])
                ret += "a";

            if (op.operations()[1])
                ret += "s";

            if (op.operations()[2])
                ret += "p";

            if (op.operations()[3])
            {
                ret += "m";
                if (typeid(*op.operations()[3]) == typeid(tape_operation_move_l_t))
                    ret += "l";
                else
                    ret += "r";
            }

            if (op.operations()[1])
            {
                ret += ' ';
                ret += dynamic_cast<const tape_operation_see_t &>(
                        *op.operations()[1]).character();
            }

            if (op.operations()[2])
            {
                ret += ' ';
                ret += dynamic_cast<const tape_operation_print_t &>(
                        *op.operations()[2]).character();
            }
        }

        else
            throw std::runtime_error{"In Machine::print_operation(const operation_t &,"
                "const encoder_t &, index_t *).\nUnknown operation.\n"};

        return ret;
    }

    void instruction_set_radix_sort(std::vector<std::shared_ptr<operation_t>> &instruction_set,
            index_t control_index, index_t n_states, index_t stride)
    {
        if (std::empty(instruction_set))
            return;

        std::vector<index_t> count(n_states, 0);
        std::vector<std::shared_ptr<operation_t>> aux(std::size(instruction_set));

        for (auto i = std::cbegin(instruction_set); i != std::cend(instruction_set); i += stride)
            ++count[dynamic_cast<const control_operation_t &>(**(i + control_index)).to()];

        for (auto i = std::begin(count) + 1; i != std::end(count); ++i)
            *i += *(i - 1);

        for (auto i = std::end(instruction_set) - stride; i >= std::begin(instruction_set);
                i -= stride)
        {
            index_t to = dynamic_cast<const control_operation_t &>(**(i + control_index)).to();

            for (auto j = i, k = std::begin(aux) + ((count[to] - 1) * stride);
                    j != i + stride; ++j, ++k)
                *k = std::move(*j);
            --count[to];
        }

        std::ranges::fill(count, 0);

        for (auto i = std::cbegin(aux); i != std::cend(aux); i += stride)
            ++count[dynamic_cast<const control_operation_t &>(**(i + control_index)).from()];

        for (auto i = std::begin(count) + 1; i != std::end(count); ++i)
            *i += *(i - 1);

        for (auto i = std::end(aux) - stride; i >= std::begin(aux); i -= stride)
        {
            index_t from = dynamic_cast<const control_operation_t &>(**(i + control_index)).from();

            for (auto j = i, k = std::begin(instruction_set) + ((count[from] - 1) * stride);
                    j != i + stride; ++j, ++k)
                *k = std::move(*j);
            --count[from];
        }

        return;
    }

    machine_t::machine_t(const machine_t &arg) :
        instruction_set_{arg.instruction_set_},
        applicable_instructions_{arg.applicable_instructions_},
        search_table_{arg.search_table_},
        output_{arg.output_},
        computation_{arg.computation_},
        computation_bits_{arg.computation_bits_},
        next_instruction_{arg.next_instruction_},
        state_{arg.state_},
        deterministic_{arg.deterministic_}
    {
        devices_.reserve(std::size(arg.devices_));

        for (const std::unique_ptr<device_t> &i : arg.devices_)
        {
            devices_.emplace_back(i->clone());

            if (arg.first_control_ == i.get())
                first_control_ = dynamic_cast<control_t *>(devices_.back().get());
        }

        return;
    }

    machine_t &machine_t::operator=(const machine_t &arg) { return *this = machine_t{arg}; }

    machine_t::machine_t(std::vector<std::unique_ptr<device_t>> devices,
            std::vector<std::shared_ptr<operation_t>> instruction_set) :
        devices_{std::move(devices)}, instruction_set_{std::move(instruction_set)}
    {
        auto throw_error = [&](const std::string &arg) -> void
        {
            throw std::runtime_error{std::string{"In Machine::machine_t::machine_t("
                "std::vector<std::unique_ptr<device_t>>, std::vector<std::shared_ptr<operation_t>>,"
                "std::unique_ptr<encoder_t>):\n"} + arg + "\n"};
        };

        index_t n = std::size(devices_);
        output_.resize(n);
        index_t s = std::size(instruction_set_);
        if (n == 0)
            throw_error("Machine without any device.");

        if (s % n != 0)
            throw_error("The number of instructions is not a multiple of the number of devices.");

        index_t control_index = 0;
        for (auto i = std::cbegin(devices_); i != std::cend(devices_);
                ++i, ++control_index)
            if (typeid(**i) == typeid(control_t))
            {
                first_control_ = dynamic_cast<control_t *>(i->get());
                break;
            }

        index_t n_states_first_control = 0;
        if (first_control_)
            n_states_first_control = first_control_->initialiser().initial_state();

        for (auto i = std::cbegin(instruction_set_); i < std::cend(instruction_set_); i += n)
        {
            auto j = i;
            for (auto k = std::cbegin(devices_); k != std::cend(devices_); ++k, ++j)
                if (not (**j).correct_device(**k))
                    throw invalid_operation_t{**j, **k};

            if (first_control_)
            {
                const control_operation_t &control_operation = 
                        dynamic_cast<const control_operation_t &>(**(i + control_index));
                n_states_first_control = std::max(n_states_first_control,
                        std::max(control_operation.from(), control_operation.to()));
            }

            for (j = std::cbegin(instruction_set_); j < i and deterministic_; j += n)
                deterministic_ = not std::equal(i, i + n, j,
                    [](const auto &a, const auto &b)
                        { return (*a).intersecting_domain(*b); });

            if (deterministic_)
                deterministic_ = not std::equal(i, i + n, std::cbegin(devices_),
                    [](const auto &a, const auto &b)
                        { return (*a).intersecting_domain((*b).terminator()); });
        }

        computation_bits_ = std::bit_width(std::size(instruction_set_) / n);

        if (computation_bits_ > std::numeric_limits<index_t>::digits)
            throw_error("The instruction set cannot have more instructions than the maximum number"
                " representable by index_t.");
        if (not first_control_)
            return;

        ++n_states_first_control;

        instruction_set_radix_sort(instruction_set_, control_index, n_states_first_control, n);

        for (auto i = std::begin(instruction_set_) + control_index; i < std::end(instruction_set_);
             i += n)
            while (dynamic_cast<control_operation_t &>(**i).from() >= std::size(search_table_))
                search_table_.emplace_back(i - control_index);
        while (n_states_first_control >= std::size(search_table_))
            search_table_.emplace_back(std::end(instruction_set_));

        return;
    }
    
    machine_t::machine_t(std::ifstream &stream)
    {
        if (not stream.is_open() or stream.bad())
            throw std::runtime_error{"In Machine::machine_t::machine_t(std::ifstream &):\n"
                "Error with the stream.\n"};

        stream.exceptions(std::ios_base::eofbit);
        index_t line = 1, column = 1;

        while(true)
        {
            std::unique_ptr<device_t> device = read_device(stream, line, column);

            if (not device)
                break;

            devices_.emplace_back(std::move(device));
        }
            
        if (std::size(devices_) == 0)
            throw std::runtime_error{"In Machine::machine_t::machine_t(std::ifstream &):\n"
                "Machine without any device.\n"};

        for (index_t i = 0; true; i = (i + 1 == std::size(devices_)) ? 0 : i + 1)
        {
            std::shared_ptr<operation_t> operation =
                read_operation(stream, line, column, *devices_[i]);

            if (not operation)
                break;

            instruction_set_.emplace_back(std::move(operation));
        }

        *this = machine_t{std::move(devices_), std::move(instruction_set_)};

        return;
    }

    void machine_t::load_program(std::ifstream &stream) { *this = machine_t{stream}; }

    void machine_t::store_program(std::ostream &arg) const
    {
        std::ostringstream stream;

        for (const auto &i : devices_)
            stream << print_device(*i) << ";\n";
        

        bool print_new_line = true;
        index_t c = 0;
        index_t last_control_state = 0;
        for (const auto &i : instruction_set_)
        {
            constexpr index_t instruction_length = 16;
            
            std::ostringstream instruction;
            index_t control_state;
            instruction << print_operation(*i, devices_[c]->encoder(), &control_state) << ';';

            if (first_control_ == devices_[c].get())
            {
                print_new_line = (control_state != last_control_state);
                last_control_state = control_state;
            }

            if (c == 0 and print_new_line)
                stream << '\n';

            if (std::size(instruction.str()) > instruction_length)
                throw std::runtime_error{"In Machine::machine_t::store_program"
                    "(std::ostream &) const\nInstruction string is too long.\n"};

            ++c;
            if (c == std::size(devices_))
            {
                c = 0;
                instruction << '\n';
            }
            else
                for (index_t i = std::size(instruction.str()); i < instruction_length; ++i)
                    instruction << ' ';
            stream << instruction.str();
        }

        arg << stream.str() << '\n';
        return;
    }

    std::vector<std::string> machine_t::print_instruction(index_t arg) const
    {
        index_t n = std::size(devices_);
        arg *= n;

        if (arg > std::size(instruction_set_))
            throw std::runtime_error{"In Machine::machine_t::print_instruction(index_t) const:\n"
                "Invalid argument.\n"};

        if (arg == std::size(instruction_set_))
            return std::vector<std::string>(std::size(devices_), "Terminate");

        auto d = std::cbegin(devices_);

        std::vector<std::string> ret;
        for (auto begin = std::cbegin(instruction_set_) + arg, end = begin + n;
             begin != end;
             ++begin, ++d)
            ret.emplace_back(print_operation(**begin, (*d)->encoder()));

        return ret;
    }

    std::vector<std::string> machine_t::print_state() const
    {
        std::vector<std::string> ret;

        for (const std::unique_ptr<device_t> &i : devices_)
            ret.emplace_back(i->print_state());

        return ret;
    }

    std::string machine_t::next_instruction(index_t arg) const
    {
        if (arg >= std::size(devices()))
            throw std::runtime_error{"In Machine::machine_t::next_instruction(index_t) const:\n"
                "Invalid argument.\n"};

        if (state_ == machine_state_t::blocked)
            return "Blocked";

        if (next_instruction_ == negative_1)
            return "";

        if (next_instruction_ == std::size(instruction_set_))
            return "Terminate";

        return print_operation(*instruction_set_[next_instruction_ + arg],
                               devices_[arg]->encoder());
    }

    const std::vector<std::unique_ptr<device_t>> &machine_t::devices() const noexcept
        { return devices_; }

    const std::vector<std::shared_ptr<operation_t>> &machine_t::instruction_set() const noexcept
        { return instruction_set_; }

    const std::vector<std::string> &machine_t::output() const noexcept { return output_; }

    bool machine_t::deterministic() const noexcept { return deterministic_; }

    machine_t::machine_state_t machine_t::state() const noexcept { return state_; }

    void machine_t::initialise(const std::string &input)
    {
        output_.clear();
        output_.resize(std::size(devices_));
        computation_.clear();
        computation_size_ = 0;

        state_ = machine_state_t::running;
        for (auto &i : devices_)
            i->initialise(input);

        applicable_instructions_apparatus();

        return;
    }

    void machine_t::initialise(std::span<const std::string> input)
    {
        if (std::size(input) == 1)
            return initialise(input.front());
        if (std::size(input) != std::size(devices_))
            throw std::runtime_error{"In Machine::machine_t::initialise(std::span<const std::string"
                ">):\nInvalid argument.\n"};

        output_.clear();
        output_.resize(std::size(devices_));
        computation_.clear();
        computation_size_ = 0;

        state_ = machine_state_t::running;
        auto i = std::begin(input);
        auto j = std::begin(devices_);
        for(; i != std::end(input); ++i, ++j)
            (*j)->initialise(*i);

        applicable_instructions_apparatus();

        return;
    }

    void machine_t::next()
    {
        if (next_instruction_ == negative_1)
            return;

        if (next_instruction_ == std::size(instruction_set_))
            terminate();

        else
        {
            auto i = std::begin(devices_);
            auto j = std::begin(instruction_set_) + next_instruction_;

            for (; i != std::end(devices_); ++i, ++j)
                (**j).apply(**i);

            computation_append();
            applicable_instructions_apparatus();
        }

        return;
    }

    void machine_t::next(index_t steps)
    {
        for (index_t i = 0; i < steps and state_ == machine_state_t::running; ++i)
            next();
        return;
    }

    void machine_t::run()
    {
        while (state_ == machine_state_t::running)
            next();
        return;
    }

    std::vector<std::string> machine_t::print_applicable_instructions() const
    {
        static constexpr index_t alignment_size = 16;

        std::vector<std::string> ret;
        
        std::vector<std::string> instructions;
        for (auto i : applicable_instructions_)
        {
            instructions = print_instruction(
                std::distance(std::begin(instruction_set_), i) / std::size(devices_));

            for (auto j = std::begin(instructions); j != std::end(instructions); ++j)
                ret.emplace_back(std::move(*j));
        }

        return ret;
    }

    void machine_t::select_instruction(index_t arg)
    {
        if (arg >= std::size(applicable_instructions_))
            throw std::runtime_error{"In Machine::machine_t::select_instruction(index_t):\n"
                "Invalid argument.\n"};

        next_instruction_ = 
            std::distance(std::cbegin(instruction_set_), applicable_instructions_[arg]);

        return;
    }

    index_t machine_t::computation(index_t arg) const
    {
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;

        index_t start_bit = arg * computation_bits_;
        index_t last_bit = start_bit + computation_bits_ - 1;

        if (last_bit >= computation_size_)
            throw std::runtime_error{"In Machine::machine_t::computation(index_t) const:\n"
                "Invalid argument.\n"};

        index_t start_word = start_bit / n_digits;
        index_t start_offset = start_bit % n_digits;
        index_t last_word = last_bit / n_digits;
        index_t last_offset = last_bit % n_digits;

        index_t ret0 = computation_[last_word];
        ret0 <<= (n_digits - 1 - last_offset);
        ret0 >>= (n_digits - computation_bits_);

        if (start_word == last_word)
            return ret0;

        index_t ret1 = computation_[start_word];
        ret1 >>= start_offset;

        return ret0 | ret1;
    }

    index_t machine_t::computation_size() const { return computation_size_ / computation_bits_; }

    bool machine_t::terminating() const
        { return std::ranges::all_of(devices_, [](const auto &i) { return i->terminating(); }); }

    void machine_t::terminate()
    {
        output_.clear();

        for (const auto &i : devices_)
            output_.emplace_back(i->terminate());
        state_ = machine_state_t::halted;
        computation_append();
        next_instruction_ = negative_1;
        applicable_instructions_.clear();
        return;
    }

    void machine_t::applicable_instructions_apparatus()
    {
        applicable_instructions_.clear();
        next_instruction_ = negative_1;

        if (state_ == machine_state_t::halted or
            state_ == machine_state_t::invalid or
            state_ == machine_state_t::blocked)
            return;

        index_t n = std::size(devices_);

        std::vector<std::shared_ptr<operation_t>>::const_iterator begin_search, end_search;

        if (not first_control_)
        {
            begin_search = std::begin(instruction_set_);
            end_search = std::end(instruction_set_);
        }
        else
        {
            index_t control_state = first_control_->state();
            begin_search = search_table_[control_state];
            end_search = search_table_[control_state + 1];
        }

        for (; begin_search < end_search; begin_search += n)
            if (std::equal(begin_search, begin_search + n, std::cbegin(devices_),
                        [](const auto &a, const auto &b)
                        { return (*a).applicable(*b); }))
            {
                applicable_instructions_.emplace_back(begin_search);
                next_instruction_ = std::distance(std::cbegin(instruction_set_), begin_search);
            }

        if (terminating())
        {
            applicable_instructions_.emplace_back(std::cend(instruction_set_));
            next_instruction_ = std::size(instruction_set_);
        }

        index_t s = std::size(applicable_instructions_);
        if (s == 0)
            state_ = machine_state_t::blocked;
        else if (s == 1)
            state_ = machine_state_t::running;
        else
        {
            next_instruction_ = negative_1;
            state_ = machine_state_t::non_deterministic_decision;
        }

        return;
    }

    void machine_t::computation_append()
    {
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;

        index_t offset = computation_size_ % n_digits;

        computation_size_ += computation_bits_;

        index_t to_append = next_instruction_ / std::size(devices_);

        if (offset == 0)
            computation_.emplace_back(to_append << offset);
        else
            computation_.back() |= (to_append << offset);

        if (offset + computation_bits_ > n_digits)
            computation_.emplace_back(to_append >> (n_digits - offset));

        return;
    }

    device_t::device_t(std::unique_ptr<encoder_t> encoder) noexcept :
        encoder_{std::move(encoder)} {}

    const encoder_t &device_t::encoder() const
    {
        if (not encoder_)
            throw std::runtime_error{"In Machine::device_t::encoder() const:\nEncoder is null.\n"};
        return *encoder_;
    }

    void device_t::initialise(const std::string &arg)
        { initialiser().initialise(*this, arg); }

    bool device_t::terminating() const
        { return terminator().terminating(*this); }

    std::string device_t::terminate()
        { return terminator().terminate(*this); }

    noop_operation_t *noop_operation_t::clone() const { return new noop_operation_t{}; }
    bool noop_operation_t::applicable(const device_t &) const { return true; }
    void noop_operation_t::apply(device_t &) const { return; }
    bool noop_operation_t::correct_device(const device_t &) const { return true; }
    bool noop_operation_t::intersecting_domain(const operation_t &) const { return true; }
    bool noop_operation_t::intersecting_domain(const terminator_t &) const { return true; }

    invalid_operation_t::invalid_operation_t(const operation_t &operation, const device_t &device) :
        operation_{operation}, device_{device} {}

    const operation_t &invalid_operation_t::operation() const { return operation_; }

    const device_t &invalid_operation_t::device() const { return device_; }

    invalid_terminator_t::invalid_terminator_t
        (const terminator_t &terminator, const device_t &device) :
        terminator_{terminator}, device_{device} {}

    const terminator_t &invalid_terminator_t::terminator() const { return terminator_; }

    const device_t &invalid_terminator_t::device() const { return device_; }
}

