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
                {arr, std::shared_ptr<operation_t>(new control_operation_t{from, to})});

        if (not a.second)
            throw std::runtime_error{"In control_operation(character_t, character_t)."};

        return a.first->second;
    }

    std::shared_ptr<operation_t> operation(character_t operation_id, character_t operation_argument)
    {
        static const std::runtime_error error{"In operation(const std::array<character_t, 2> &)."};
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
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        noop_operation_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 8:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        input_operation_scan_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 9:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        input_operation_next_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 10:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        input_operation_eof_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 11:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        output_operation_write_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 12:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_push_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 13:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_pop_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 14:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_top_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 15:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_empty_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 16:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_inc_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 17:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_dec_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 18:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_zero_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 19:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_non_zero_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 20:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_inc_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 21:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_dec_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 22:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_zero_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 23:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_pos_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 24:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_neg_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 25:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_see_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 26:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_print_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 27:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_move_l_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 28:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_move_r_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 29:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_athome_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 30:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        queue_operation_enqueue_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 31:
            {
                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        queue_operation_dequeue_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            case 32:
            {
                if (arg[1] != 0)
                    throw error;

                auto a = map.insert(std::pair<std::array<character_t, 2>,
                                    std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        queue_operation_empty_t{})});
                if (not a.second)
                    throw error;
                return a.first->second;
            }

            default:
                throw error;
        }
    }

    std::shared_ptr<operation_t> compound_tape_operation(character_t flag, 
            character_t see_character, character_t print_character)
    {
        static const std::string error{
            "In compound_tape_operation(character_t, character_t, character_t).\n"};
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
                arg, std::shared_ptr<operation_t>(new tape_operation_compound_t{operations_})});
        if (not a.second)
            throw std::runtime_error{error + "Insertion in hash map failed.\n"};
        return a.first->second;
    }

    std::string read_token(std::istream &stream)
    {
        std::string ret{};

        char a;
        stream.get(a);
        while (std::isspace(a))
            stream.get(a);
         
        while (a == '/')
        {
            while (a < 10 or a > 13)
                stream.get(a);
            while (std::isspace(a))
                stream.get(a);
        }
        ret += a;

        if (std::ispunct(a))
            return ret;

        stream.get(a);
        while(not std::ispunct(a) and not std::isspace(a))
        {
            ret += a;
            stream.get(a);
        }
        stream.unget();

        return ret;
    }

    character_t read_character(std::istream &stream, const encoder_t &encoder)
    {
        auto initial_pos = stream.tellg();

        std::string token;
        try
        {
            token = read_token(stream);
        }
        catch (...)
        {
            stream.seekg(initial_pos);
            throw;
        }

        if (std::size(token) != 1)
        {
            stream.seekg(initial_pos);
            throw std::runtime_error{"In read_character(std::istream &).\n"
                "Expected single character, received string '" + token + "'.\n"};
        }

        character_t ret;

        try
        {
            ret = encoder(token[0]);
        }
        catch (...)
        {
            stream.seekg(initial_pos);
            throw;
        }

        return ret;
    }


    bool is_number(const std::string &arg)
        { return std::ranges::all_of(arg, [](char a){ return a >= '0' and a <= '9'; }); }

    std::unique_ptr<encoder_t> read_encoder(std::istream &stream)
    {
        static const std::runtime_error error{
            "In read_encoder(std::istream &):\nInvalid input"};

        std::unique_ptr<encoder_t> ret;
        std::string token = read_token(stream);
        if (token == "alphabetic")
        {
            token = read_token(stream);
            if (not is_number(token))
                throw error;
            character_t c = static_cast<character_t>(std::stoll(token));

            token = read_token(stream);
            if (token != ";")
                throw error;

            ret = std::unique_ptr<encoder_t>{new encoder_alphabetic_t{c}};
        }

        else if (token == "list")
        {
            std::vector<char> list;
            
            for (token = read_token(stream); token != ";"; token = read_token(stream))
            {
                if (std::size(token) != 1)
                    throw error;
                list.emplace_back(token[0]);
            }

            ret = std::unique_ptr<encoder_t>{new encoder_list_t{std::move(list)}};
        }

        else if (token == "ascii")
        {
            token = read_token(stream);
            if (token != ";")
                throw error;

            ret = std::unique_ptr<encoder_t>{new encoder_ascii_t{}};
        }

        else
           throw error;

       return ret; 
    }

    std::string print_encoder(const encoder_t &encoder)
    {
        static const std::runtime_error error{"In print_encoder(const encoder_t &)."};

        std::string ret;
        if (typeid(encoder) == typeid(encoder_alphabetic_t))
        {
            ret = "alphabetic ";
            ret += std::to_string(encoder.alphabet().n_characters());
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
        {
            ret = "ascii";
        }

        else
            throw error;

        return ret;
    }

    std::unique_ptr<device_t> read_device(std::istream &stream, const encoder_t &encoder)
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
        const alphabet_t &alphabet = encoder.alphabet();

        try
        {
            token = read_token(stream);
        }

        catch (std::ios_base::failure &)
        {
            return {nullptr};
        }


        const auto it = keywords.find(token);
        if (it == std::end(keywords))
        {
            stream.seekg(initial_pos);
            return {nullptr};
        }

        switch (it->second)
        {
            case 0:
            {
                index_t initial_state;
                token = read_token(stream);
                if (not is_number(token))
                    throw std::runtime_error{error + "The first token after control must be"
                        " a number (the initial state).\n"};
                initial_state = static_cast<index_t>(std::stoll(token));

                token = read_token(stream);

                std::unordered_map<index_t, string_t> terminating_states;
                while (token != ";")
                {
                    if (token != ":")
                    throw std::runtime_error{error + "Reading a control: expected ':' got '" +
                        token + "'\n"};

                    std::string terminator_string = read_token(stream);
                    token = read_token(stream);
                    while (token != ":" and token != ";")
                    {
                        if (not is_number(token))
                            throw std::runtime_error{error + "Reading a control: expected number"
                                " got '" + token + "'\n"};

                        index_t terminating_state = static_cast<index_t>(std::stoll(token));
                        terminating_states[terminating_state] = terminator_string;
                        token = read_token(stream);
                    }
                }
                control_initialiser_t initialiser{initial_state};
                control_terminator_t terminator{std::move(terminating_states)};

                return std::unique_ptr<device_t>(new control_t
                        {std::move(initialiser), std::move(terminator)});
            }

            case 1:
            {
                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading an input: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new input_t{alphabet});
            }

            case 2:
            {
                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading an output: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new output_t{alphabet});
            }

            case 3:
            {
                std::unique_ptr<stack_initialiser_t> initialiser;
                token = read_token(stream);
                if (token == "empty")
                    initialiser.reset(new stack_initialiser_empty_t{});
                else if (token == "string")
                    initialiser.reset(new stack_initialiser_string_t{});
                else
                    throw std::runtime_error{error + "Reading a stack: expected the initialiser "
                        "'empty' or 'string' got '" + token + "'\n"};

                std::unique_ptr<stack_terminator_t> terminator;
                token = read_token(stream);
                if (token == "empty")
                    terminator.reset(new stack_terminator_empty_t{});
                else if (token == "string")
                    terminator.reset(new stack_terminator_string_t{});
                else
                    throw std::runtime_error{error + "Reading a stack: expected the terminator "
                        "'empty' or 'string' got '" + token + "'\n"};

                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading a stack: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new stack_t{alphabet,
                        std::move(initialiser), std::move(terminator)});
            }

            case 4:
            {
                std::unique_ptr<unsigned_counter_initialiser_t> initialiser;
                token = read_token(stream);
                if (token == "zero")
                    initialiser.reset(new unsigned_counter_initialiser_zero_t{});
                else if (token == "ascii")
                    initialiser.reset(new unsigned_counter_initialiser_ascii_t{});
                else if (token == "b_ary")
                    initialiser.reset(new unsigned_counter_initialiser_b_ary_t{});
                else if (token == "b_adic")
                    initialiser.reset(new unsigned_counter_initialiser_b_adic_t{});
                else
                    throw std::runtime_error{error + "Reading an ucounter: expected initialiser "
                       "'ascii', 'b_ary' or 'b_adic' got '" + token + "'\n"};

                std::unique_ptr<unsigned_counter_terminator_t> terminator;
                token = read_token(stream);
                if (token == "zero")
                    terminator.reset(new unsigned_counter_terminator_zero_t{});
                else if (token == "string")
                    terminator.reset(new unsigned_counter_terminator_string_t{});
                else
                    throw std::runtime_error{error + "Reading an ucounter: expected terminator "
                       "'zero' or 'string' got '" + token + "'\n"};

                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading an ucounter: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new unsigned_counter_t{std::move(initialiser),
                        std::move(terminator)});
            }

            case 5:
            {
                std::unique_ptr<counter_initialiser_t> initialiser;
                token = read_token(stream);
                if (token == "zero")
                    initialiser.reset(new counter_initialiser_zero_t{});
                else if (token == "ascii")
                    initialiser.reset(new counter_initialiser_ascii_t{});
                else if (token == "b_ary")
                    initialiser.reset(new counter_initialiser_b_ary_t{});
                else if (token == "b_adic")
                    initialiser.reset(new counter_initialiser_b_adic_t{});
                else
                    throw std::runtime_error{error + "Reading a counter: expected initialiser "
                       "'ascii' 'b_ary' or 'b_adic' got '" + token + "'\n"};

                std::unique_ptr<counter_terminator_t> terminator;
                token = read_token(stream);
                if (token == "zero")
                    terminator.reset(new counter_terminator_zero_t{});
                else if (token == "string")
                    terminator.reset(new counter_terminator_string_t{});
                else
                    throw std::runtime_error{error + "Reading a counter: expected terminator "
                       "'zero' or 'string' got '" + token + "'\n"};

                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading a counter: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new counter_t{std::move(initialiser),
                        std::move(terminator)});
            }

            case 6:
            {
                character_t c;
                try
                {
                    c = read_character(stream, encoder);
                }
                catch (std::runtime_error &)
                {
                    token = read_token(stream);
                    throw std::runtime_error{error + "Reading a tape: expected the default"
                        " character got '" + token + "'\n"};
                }

                std::unique_ptr<tape_initialiser_t> initialiser;
                token = read_token(stream);
                if (token == "empty")
                    initialiser.reset(new tape_initialiser_empty_t{});
                else if (token == "string")
                    initialiser.reset(new tape_initialiser_string_t{});
                else
                    throw std::runtime_error{error + "Reading a tape: expected initialiser "
                       "'empty' or 'string' got '" + token + "'\n"};

                std::unique_ptr<tape_terminator_t> terminator;
                token = read_token(stream);
                if (token == "athome")
                    terminator.reset(new tape_terminator_athome_t{});
                else if (token == "always")
                    terminator.reset(new tape_terminator_always_t{});
                else
                    throw std::runtime_error{error + "Reading a tape: expected terminator "
                       "'athome' or 'always' got '" + token + "'\n"};

                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading a tape: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new tape_t{alphabet, c,
                        std::move(initialiser), std::move(terminator)});
            }

            case 7:
            {
                std::unique_ptr<queue_initialiser_t> initialiser;
                token = read_token(stream);
                if (token == "empty")
                    initialiser.reset(new queue_initialiser_empty_t{});
                else if (token == "string")
                    initialiser.reset(new queue_initialiser_string_t{});
                else
                    throw std::runtime_error{error + "Reading a queue: expected initialiser "
                       "'empty' or 'string' got '" + token + "'\n"};

                std::unique_ptr<queue_terminator_t> terminator;
                token = read_token(stream);
                if (token == "empty")
                    terminator.reset(new queue_terminator_empty_t{});
                else if (token == "string")
                    terminator.reset(new queue_terminator_string_t{});
                else
                    throw std::runtime_error{error + "Reading a queue: expected terminator "
                       "'empty' or 'string' got '" + token + "'\n"};

                token = read_token(stream);
                if (token != ";")
                    throw std::runtime_error{error + "Reading a queue: expected ';' got '" +
                        token + "'\n"};

                return std::unique_ptr<device_t>(new queue_t{alphabet,
                        std::move(initialiser), std::move(terminator)});
            }
        }
        
        throw std::runtime_error{error + "Unknown device.\n"};
    }

    std::string print_device(const device_t &device)
    {
        static const std::runtime_error error{"In print_device(const device_t &)."};

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
                    ret += ": " + last_string->to_ascii() + ", ";
                else
                    ret += ", ";
                ret += std::to_string(i.second);
            }
        }

        else if (typeid(device) == typeid(input_t))
            ret += "input";

        else if (typeid(device) == typeid(output_t))
        {
            ret += "output ";
            ret += std::to_string(dynamic_cast<const output_t &>(device).alphabet().n_characters());
        }

        else if (typeid(device) == typeid(stack_t))
        {
            const stack_t &stack = dynamic_cast<const stack_t &>(device);
            const stack_initialiser_t &initialiser = stack.initialiser();
            const stack_terminator_t &terminator = stack.terminator();

            ret += "stack " + std::to_string(stack.alphabet().n_characters());
            ret += " ";

            if (typeid(initialiser) == typeid(stack_initialiser_empty_t))
                ret += "empty ";
            else if (typeid(initialiser) == typeid(stack_initialiser_string_t))
                ret += "string ";
            else
                throw error;

            if (typeid(terminator) == typeid(stack_terminator_empty_t))
                ret += "empty";
            else if (typeid(terminator) == typeid(stack_terminator_string_t))
                ret += "string";
            else
                throw error;
        }

        else if (typeid(device) == typeid(unsigned_counter_t))
        {
            const unsigned_counter_t &unsigned_counter =
                dynamic_cast<const unsigned_counter_t &>(device);
            const unsigned_counter_initialiser_t &initialiser = unsigned_counter.initialiser();
            const unsigned_counter_terminator_t &terminator = unsigned_counter.terminator();

            ret += "ucounter ";

            if (typeid(initialiser) == typeid(unsigned_counter_initialiser_zero_t))
                ret += "zero ";
            else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_ascii_t))
                ret += "ascii ";
            else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_b_ary_t))
                ret += "b_ary ";
            else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_b_adic_t))
                ret += "b_adic ";
            else
                throw error;

            if (typeid(terminator) == typeid(unsigned_counter_terminator_zero_t))
                ret += "zero";
            else if (typeid(terminator) == typeid(unsigned_counter_terminator_string_t))
                ret += "string";
            else
                throw error;
        }

        else if (typeid(device) == typeid(counter_t))
        {
            const counter_t &counter = dynamic_cast<const counter_t &>(device);
            const counter_initialiser_t &initialiser = counter.initialiser();
            const counter_terminator_t &terminator = counter.terminator();

            ret += "counter ";

            if (typeid(initialiser) == typeid(counter_initialiser_zero_t))
                ret += "zero ";
            else if (typeid(initialiser) == typeid(counter_initialiser_ascii_t))
                ret += "ascii ";
            else if (typeid(initialiser) == typeid(counter_initialiser_b_ary_t))
                ret += "b_ary ";
            else if (typeid(initialiser) == typeid(counter_initialiser_b_adic_t))
                ret += "b_adic ";
            else
                throw error;

            if (typeid(terminator) == typeid(counter_terminator_zero_t))
                ret += "zero";
            else if (typeid(terminator) == typeid(counter_terminator_string_t))
                ret += "string";
            else
                throw error;
        }

        else if (typeid(device) == typeid(tape_t))
        {
            const tape_t &tape = dynamic_cast<const tape_t &>(device);
            const tape_initialiser_t &initialiser = tape.initialiser();
            const tape_terminator_t &terminator = tape.terminator();

            ret += "tape " + std::to_string(tape.alphabet().n_characters());
            ret += " ";
            ret += std::to_string(tape.default_character());
            ret += " ";

            if (typeid(initialiser) == typeid(tape_initialiser_empty_t))
                ret += "empty ";
            else if (typeid(initialiser) == typeid(tape_initialiser_string_t))
                ret += "string ";

            if (typeid(terminator) == typeid(tape_terminator_athome_t))
                ret += "athome";
            else if (typeid(terminator) == typeid(tape_terminator_always_t))
                ret += "always";
        }

        else if (typeid(device) == typeid(queue_t))
        {
            const queue_t &queue = dynamic_cast<const queue_t &>(device);
            const queue_initialiser_t &initialiser = queue.initialiser();
            const queue_terminator_t &terminator = queue.terminator();

            ret += "queue " + std::to_string(queue.alphabet().n_characters());
            ret += " ";

            if (typeid(initialiser) == typeid(queue_initialiser_empty_t))
                ret += "empty ";
            else if (typeid(initialiser) == typeid(queue_initialiser_string_t))
                ret += "string ";
            else
                throw error;

            if (typeid(terminator) == typeid(queue_terminator_empty_t))
                ret += "empty";
            else if (typeid(terminator) == typeid(queue_terminator_string_t))
                ret += "string";
            else
                throw error;
        }

        else
            throw error;

        return ret;
    }

    std::shared_ptr<operation_t> read_operation
        (std::istream &stream, const device_t &device, const encoder_t &encoder)
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
            //keywords["uinc"] = 16; -> 20
            //keywords["udec"] = 17; -> 21
            //keywords["uzero"] = 18; -> 22
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
            //keywords["qempty"] = 32; -> 15
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

        std::string token;

        try
        {
            token = read_token(stream);
        }
        catch (std::ios_base::failure &)
        {
            return {nullptr};
        }

        if (is_number(token))
        {
            if (typeid(device) != typeid(control_t))
                throw std::runtime_error{error + "Incorrect device for control operation.\n"
                    "Expected control_t.\n"};
            
            character_t from = static_cast<character_t>(std::stoll(token));

            token = read_token(stream);
            if (token != "to")
                throw std::runtime_error{error + "Incorrect token while reading control operation;"
                    " Expected keyword 'to'.\n"};

            token = read_token(stream);
            if (not is_number(token))
                throw std::runtime_error{error + "Incorrect token while reading control operation;"
                    " Expected a number.\n"};
            character_t to = static_cast<character_t>(std::stoll(token));

            token = read_token(stream);
            if (token != ";")
                throw std::runtime_error{error + "Missing ';'"};

            return control_operation(from, to);
        }

        else
        {
            auto it = keywords.find(token);
            if (it == std::end(keywords))
                throw std::runtime_error{error + "Unknown instruction.\n"};

            switch (it->second)
            {
                case 7:
                {
                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(7, 0);
                }

                case 8:
                {
                    if (typeid(device) != typeid(input_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected input_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(8, c);
                }

                case 9:
                {
                    if (typeid(device) != typeid(input_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected input_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(9, c);
                }

                case 10:
                {
                    if (typeid(device) != typeid(input_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected input_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(10, 0);
                }

                case 11:
                {
                    if (typeid(device) != typeid(output_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected output_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(11, c);
                }

                case 12:
                {
                    if (typeid(device) != typeid(stack_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected stack_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(12, c);
                }

                case 13:
                {
                    if (typeid(device) != typeid(stack_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected stack_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(13, c);
                }

                case 14:
                {
                    if (typeid(device) != typeid(stack_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected stack_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(14, c);
                }

                case 15:
                {
                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    if (typeid(device) == typeid(stack_t))
                        return operation(15, 0);

                    if (typeid(device) == typeid(queue_t))
                        return operation(32, 0);

                    throw std::runtime_error{error + "Incorrect device for operation '" + token
                            + "'.\nExpected stack_t or queue_t.\n"};
                }

                case 19:
                {
                    if (typeid(device) != typeid(unsigned_counter_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected unsigned_counter_t.\n"};

                    return operation(19, 0);
                }

                case 20:
                {
                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    if (typeid(device) == typeid(counter_t))
                        return operation(20, 0);

                    if (typeid(device) == typeid(unsigned_counter_t))
                        return operation(16, 0);

                    throw std::runtime_error{error + "Incorrect device for operation '" + token
                            + "'.\nExpected counter_t or unsigned_counter_t.\n"};
                }

                case 21:
                {
                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    if (typeid(device) == typeid(counter_t))
                        return operation(21, 0);

                    if (typeid(device) == typeid(unsigned_counter_t))
                        return operation(17, 0);

                    throw std::runtime_error{error + "Incorrect device for operation '" + token
                            + "'.\nExpected counter_t or unsigned_counter_t.\n"};
                }

                case 22:
                {
                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    if (typeid(device) == typeid(counter_t))
                        return operation(22, 0);

                    if (typeid(device) == typeid(unsigned_counter_t))
                        return operation(18, 0);

                    throw std::runtime_error{error + "Incorrect device for operation '" + token
                            + "'.\nExpected counter_t or unsigned_counter_t.\n"};
                }

                case 23:
                {
                    if (typeid(device) != typeid(counter_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected counter_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(23, 0);
                }

                case 24:
                {
                    if (typeid(device) != typeid(counter_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected counter_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(24, 0);
                }

                case 25:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(25, c);
                }

                case 26:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(26, c);
                }

                case 27:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(27, 0);
                }

                case 28:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(28, 0);
                }

                case 29:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(29, 0);
                }

                case 30:
                {
                    if (typeid(device) != typeid(queue_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected queue_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(30, c);
                }

                case 31:
                {
                    if (typeid(device) != typeid(queue_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected queue_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return operation(31, c);
                }

                case 33:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(12, 0, c);
                }

                case 34:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(20, 0, c);
                }

                case 35:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(10, c, 0);
                }

                case 36:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(18, c, 0);
                }

                case 37:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);
                    character_t d = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(6, c, d);
                }

                case 38:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);
                    character_t d = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(14, c, d);
                }

                case 39:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);
                    character_t d = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(22, c, d);
                }

                case 40:
                {
                    throw std::runtime_error{error + "Inconsistent compound instruction '" + token +
                        "'; athome and move_l present.\n"};
                }

                case 41:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(17, 0, 0);
                }

                case 42:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(5, 0, c);
                }

                case 43:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(21, 0, c);
                }

                case 44:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(3, c, 0);
                }

                case 45:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(19, c, 0);
                }

                case 46:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);
                    character_t d = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(7, c, d);
                }

                case 47:
                {
                    if (typeid(device) != typeid(tape_t))
                        throw std::runtime_error{error + "Incorrect device for operation '" + token
                                + "'.\nExpected tape_t.\n"};

                    character_t c = read_character(stream, encoder);
                    character_t d = read_character(stream, encoder);

                    token = read_token(stream);
                    if (token != ";")
                        throw std::runtime_error{error + "Missing ';'"};

                    return compound_tape_operation(23, c, d);
                }

                default:
                    throw std::runtime_error{error + "Unknown instruction.\n"};
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
            throw std::runtime_error{"In print_operation(const operation_t &, const encoder_t &,"
                "index_t *).\nUnknown operation.\n"};

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

    machine_t::machine_t(std::vector<std::unique_ptr<device_t>> devices,
            std::vector<std::shared_ptr<operation_t>> instruction_set,
            const encoder_t &encoder) :
        machine_t{std::move(devices), std::move(instruction_set),
            std::unique_ptr<encoder_t>(encoder.clone())} {}

    machine_t::machine_t(std::vector<std::unique_ptr<device_t>> devices,
            std::vector<std::shared_ptr<operation_t>> instruction_set,
            std::unique_ptr<encoder_t> encoder) :
        devices_{std::move(devices)}, instruction_set_{std::move(instruction_set)},
        encoder_{std::move(encoder)}
    {
        index_t n = std::size(devices_);
        index_t s = std::size(instruction_set_);
        if (n == 0 or s % n != 0)
            throw std::runtime_error
                {"In machine_t::machine_t(std::vector<device_t>, std::vector<operation_t>)."};

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
    
    machine_t::machine_t(std::istream &stream)
    {
        static const std::runtime_error error{
            "In machine_t::machine_t(std::istream &):\nInvalid input"};

        stream.exceptions(std::ios_base::eofbit);

        encoder_ = read_encoder(stream);

        while(true)
        {
            std::unique_ptr<device_t> device = read_device(stream, encoder());

            if (not device)
                break;

            devices_.emplace_back(std::move(device));
        }
            
        if (std::size(devices_) == 0)
            throw error;

        for (index_t i = 0; true; i = (i + 1 == std::size(devices_)) ? 0 : i + 1)
        {
            std::shared_ptr<operation_t> operation =
                read_operation(stream, *devices_[i], encoder());

            if (not operation)
                break;

            instruction_set_.emplace_back(std::move(operation));
        }

        *this = machine_t{std::move(devices_), std::move(instruction_set_),
            std::move(encoder_)};

        return;
    }

    machine_t &machine_t::load(std::istream &stream)
        { return *this = machine_t{stream}; }

    void machine_t::store(std::ostream &arg) const
    {
        std::ostringstream stream;
        static const std::runtime_error error{"In machine_t::store(std::ostream &)."};

        stream << print_encoder(encoder()) << ";\n";

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
            instruction << print_operation(*i, encoder(), &control_state) << ';';

            if (first_control_ == devices_[c].get())
            {
                print_new_line = (control_state != last_control_state);
                last_control_state = control_state;
            }

            if (c == 0 and print_new_line)
                stream << '\n';

            if (std::size(instruction.str()) > instruction_length)
                throw error; 

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

    const encoder_t &machine_t::encoder() const { return *encoder_; }
    const alphabet_t &machine_t::alphabet() const { return encoder().alphabet(); }

    std::span<const string_t> machine_t::output() const
        { return {std::begin(output_), std::end(output_)}; }

    bool machine_t::deterministic() const { return deterministic_; }

    machine_t::machine_state_t machine_t::state() const { return state_; }

    void machine_t::initialise(const std::string &input)
    {
        string_t string{alphabet()};
        for (auto i = std::crbegin(input); i != std::crend(input); ++i)
            string.push(encoder()(*i));
        initialise(string);
        return;
    }

    void machine_t::initialise(const string_t &input)
    {
        if (input.alphabet() != alphabet())
            throw invalid_alphabet_t{};

        output_.clear();
        for (auto &device : devices_)
            device->initialise(input);

        state_ = machine_state_t::running;
        applicable_instructions_apparatus();
        return;
    }

    void machine_t::next()
    {
        if (state_ != machine_state_t::running)
            return;

        if (terminating())
            terminate();

        else
        {
            auto i = std::begin(devices_);
            auto j = std::cbegin(applicable_instructions_);

            for (; i != std::end(devices_); ++i, ++j)
                (**j).apply(**i);
        }

        applicable_instructions_apparatus();
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

    std::vector<std::span<const std::shared_ptr<operation_t>>>
        machine_t::applicable_instructions() const
    {
        std::vector<std::span<const std::shared_ptr<operation_t>>> ret;
        index_t n = std::size(devices_);
        for (auto i = std::cbegin(applicable_instructions_);
             i < std::cend(applicable_instructions_); i += n)
            ret.emplace_back(i, i + n);
        return ret;
    }

    bool machine_t::terminating() const
    {
        return std::ranges::all_of(devices_, [](const auto &i) { return i->terminating(); });
    }

    void machine_t::terminate()
    {
        for (const auto &i : devices_)
            output_.emplace_back(i->terminate());
        state_ = machine_state_t::halted;
        applicable_instructions_.clear();
        return;
    }

    void machine_t::applicable_instructions_apparatus()
    {
        applicable_instructions_.clear();

        if (state_ == machine_state_t::halted or state_ == machine_state_t::invalid)
            return;

        index_t n = std::size(devices_);

        decltype(std::begin(instruction_set_)) begin_search, end_search;

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
                for (auto i = begin_search; i != begin_search + n; ++i)
                    applicable_instructions_.emplace_back((*i)->clone());

        index_t s = std::size(applicable_instructions_);
        if (terminating())
            s += n;

        if (s == 0)
            state_ = machine_state_t::blocked;
        else if (s == n)
            state_ = machine_state_t::running;
        else
            state_ = machine_state_t::non_deterministic_decision;

        return;
    }

    void device_t::initialise(const string_t &arg)
        { initialiser().initialise(*this, arg); return; }

    bool device_t::terminating() const
        { return terminator().terminating(*this); }

    string_t device_t::terminate()
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

