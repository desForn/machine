#include "machine.hpp"
#include "control.hpp"
#include "input.hpp"
#include "output.hpp"
#include "stack.hpp"
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
    struct hash
    {
        std::size_t operator()(const std::array<index_t, 2> &arg) const
        {
            if constexpr (sizeof(index_t) < 8)
                return arg[0] ^ (arg[1] + 0x9e3779b9 + (arg[0] << 6) + (arg[0] >> 2));
            else
                return arg[0] ^ (arg[1] + 0x517cc1b727220a95 + (arg[0] << 6) + (arg[0] >> 2));
        }
    };

    std::shared_ptr<operation_t> control_operation(index_t from, index_t to)
    {
        static std::unordered_map<std::array<index_t, 2>, std::shared_ptr<operation_t>, hash> map;

        const std::array<index_t, 2> arr{from, to};
        auto it = map.find(arr);

        if (it != std::end(map))
            return it->second;

        auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>
                {arr, std::shared_ptr<operation_t>(new control_operation_t{from, to})});

        if (not a.second)
            throw std::runtime_error{"In control_operation(index_t, index_t)."};

        return a.first->second;
    }

    std::shared_ptr<operation_t> operation(index_t operation_id, index_t operation_argument)
    {
        static std::unordered_map<std::array<index_t, 2>, std::shared_ptr<operation_t>, hash> map;

        const std::array<index_t, 2> arg{operation_id, operation_argument};
        auto it = map.find(arg);

        if (it != std::end(map))
            return it->second;

        switch (arg[0])
        {
            case 8:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        input_operation_scan_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 9:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        input_operation_next_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 10:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        input_operation_eof_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 11:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        output_operation_write_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 12:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_push_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 13:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_pop_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 14:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_top_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 15:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        stack_operation_empty_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 16:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_inc_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 17:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_dec_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 18:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_zero_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 19:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        unsigned_counter_operation_non_zero_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 20:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_inc_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 21:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_dec_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 22:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_zero_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 23:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_pos_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 24:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        counter_operation_neg_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 25:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_see_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 26:
            {
                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_print_t{static_cast<character_t>(arg[1])})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 27:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_move_l_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 28:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_move_r_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            case 29:
            {
                if (arg[1] != 0)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};

                auto a = map.insert(std::pair<std::array<index_t, 2>, std::shared_ptr<operation_t>>{
                        arg, std::shared_ptr<operation_t>(new
                        tape_operation_athome_t{})});
                if (not a.second)
                    throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
                return a.first->second;
            }

            default:
                throw std::runtime_error{"In operation(const std::array<index_t, 2> &)."};
        }
    }

    void instruction_set_radix_sort(std::vector<std::shared_ptr<operation_t>> &instruction_set,
            index_t control_index, index_t n_states, index_t stride)
    {
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

    bool is_number(const std::string &arg)
        { return std::ranges::all_of(arg, [](char a){ return a >= '0' and a <= '9'; }); }

    machine_t::machine_t(std::vector<std::unique_ptr<device_t>> devices,
            std::vector<std::shared_ptr<operation_t>> instruction_set, alphabet_t alphabet) :
        devices_{std::move(devices)}, instruction_set_{std::move(instruction_set)},
        alphabet_{alphabet}
    {
        index_t n = std::size(devices_);
        index_t s = std::size(instruction_set_);
        if (s == 0 or s % n != 0)
            throw std::invalid_argument
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
        static bool initialised = false;
        static std::unordered_map<std::string, index_t> keywords;
        static const std::runtime_error input_error{
            "In machine_t::machine_t(std::istream &):\nInvalid input"};

        if (not initialised)
        {
            keywords["control"] = 0;
            keywords["input"] = 1;
            keywords["output"] = 2;
            keywords["stack"] = 3;
            keywords["ucounter"] = 4;
            keywords["counter"] = 5;
            keywords["tape"] = 6;

            keywords["to"] = 7;
            keywords["scan"] = 8;
            keywords["next"] = 9;
            keywords["eof"] = 10;
            keywords["write"] = 11;
            keywords["push"] = 12;
            keywords["pop"] = 13;
            keywords["top"] = 14;
            keywords["empty"] = 15;
            keywords["uinc"] = 16;
            keywords["udec"] = 17;
            keywords["uzero"] = 18;
            keywords["unonzero"] = 19;
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

            initialised = true;
        }

        try
        {
            stream.exceptions(std::ios_base::eofbit);
            std::string token;

            token = read_token(stream);
            if (not is_number(token))
                throw input_error;
            alphabet_ = alphabet_t{static_cast<character_t>(std::stoll(token))};

            token = read_token(stream);
            if (token != ";")
                throw input_error;

            token = read_token(stream);
            while(true)
            {
                if (is_number(token))
                    break;
                auto it = keywords.find(token);
                if (it == std::end(keywords))
                    throw input_error;
                if (it->second > 6)
                    break;

                switch (it->second)
                {
                    case 0:
                    {
                        index_t initial_state;
                        token = read_token(stream);
                        if (not is_number(token))
                            throw input_error;
                        initial_state = static_cast<index_t>(std::stoll(token));

                        token = read_token(stream);

                        std::unordered_map<index_t, string_t> terminating_states;
                        while (token != ";")
                        {
                            if (token != ":")
                                throw input_error;

                            std::string terminator_string = read_token(stream);
                            token = read_token(stream);
                            while (token != ":" and token != ";")
                            {
                                if (token != ",")
                                    throw input_error;
                                token = read_token(stream);
                                if (not is_number(token))
                                    throw input_error;

                                index_t terminating_state = static_cast<index_t>(std::stoll(token));
                                terminating_states[terminating_state] = terminator_string;
                                token = read_token(stream);
                            }
                        }
                        control_initialiser_t initialiser{initial_state};
                        control_terminator_t terminator{std::move(terminating_states)};
                        devices_.emplace_back(new control_t
                                {std::move(initialiser), std::move(terminator)});

                        break;
                    }

                    case 1:
                    {
                        token = read_token(stream);
                        if (not is_number(token))
                            throw input_error;
                        devices_.emplace_back(new input_t{alphabet_t
                                {static_cast<character_t>(std::stoll(token))}});
                        token = read_token(stream);
                        if (token != ";")
                            throw input_error;

                        break;
                    }

                    case 2:
                    {
                        token = read_token(stream);
                        if (not is_number(token))
                            throw input_error;
                        devices_.emplace_back(new output_t{alphabet_t
                                {static_cast<character_t>(std::stoll(token))}});
                        token = read_token(stream);
                        if (token != ";")
                            throw input_error;

                        break;
                    }

                    case 3:
                    {
                        token = read_token(stream);
                        if (not is_number(token))
                            throw input_error;
                        alphabet_t alphabet{static_cast<character_t>(std::stoll(token))};

                        std::unique_ptr<stack_initialiser_t> initialiser;
                        token = read_token(stream);
                        if (token == "empty")
                            initialiser.reset(new stack_initialiser_empty_t{});
                        else if (token == "string")
                            initialiser.reset(new stack_initialiser_string_t{});
                        else
                            throw input_error;

                        std::unique_ptr<stack_terminator_t> terminator;
                        token = read_token(stream);
                        if (token == "empty")
                            terminator.reset(new stack_terminator_empty_t{});
                        else if (token == "string")
                            terminator.reset(new stack_terminator_string_t{});
                        else
                            throw input_error;

                        token = read_token(stream);
                        if (token != ";")
                            throw input_error;

                        devices_.emplace_back(new stack_t{alphabet,
                                std::move(initialiser), std::move(terminator)});

                        break;
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
                            throw input_error;

                        std::unique_ptr<unsigned_counter_terminator_t> terminator;
                        token = read_token(stream);
                        if (token == "zero")
                            terminator.reset(new unsigned_counter_terminator_zero_t{});
                        else if (token == "string")
                            terminator.reset(new unsigned_counter_terminator_string_t{});
                        else
                            throw input_error;

                        token = read_token(stream);
                        if (token != ";")
                            throw input_error;

                        devices_.emplace_back(new unsigned_counter_t{std::move(initialiser),
                                std::move(terminator)});

                        break;
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
                            throw input_error;

                        std::unique_ptr<counter_terminator_t> terminator;
                        token = read_token(stream);
                        if (token == "zero")
                            terminator.reset(new counter_terminator_zero_t{});
                        else if (token == "string")
                            terminator.reset(new counter_terminator_string_t{});
                        else
                            throw input_error;

                        token = read_token(stream);
                        if (token != ";")
                            throw input_error;

                        devices_.emplace_back(new counter_t{std::move(initialiser),
                                std::move(terminator)});

                        break;
                    }

                    case 6:
                    {
                        token = read_token(stream); 
                        if (not is_number(token))
                            throw input_error;
                        alphabet_t alphabet{static_cast<character_t>(std::stoll(token))};

                        token = read_token(stream); 
                        if (not is_number(token))
                            throw input_error;
                        character_t character{static_cast<character_t>(std::stoll(token))};

                        if (character >= alphabet.n_characters())
                            throw input_error;

                        std::unique_ptr<tape_initialiser_t> initialiser;
                        token = read_token(stream);
                        if (token == "empty")
                            initialiser.reset(new tape_initialiser_empty_t{});
                        else if (token == "string")
                            initialiser.reset(new tape_initialiser_string_t{});
                        else
                            throw input_error;

                        std::unique_ptr<tape_terminator_t> terminator;
                        token = read_token(stream);
                        if (token == "empty")
                            terminator.reset(new tape_terminator_empty_t{});
                        else if (token == "string")
                            terminator.reset(new tape_terminator_string_t{});
                        else
                            throw input_error;

                        token = read_token(stream);
                        if (token != ";")
                            throw input_error;

                        devices_.emplace_back(new tape_t{alphabet, character,
                                std::move(initialiser), std::move(terminator)});

                        break;
                    }
                }

                try
                {
                    token = read_token(stream);
                }
                catch (std::ios_base::failure &)
                {
                    *this = machine_t{std::move(devices_), std::move(instruction_set_),
                        std::move(alphabet_)};
                    return;
                }
            }

            if (std::size(devices_) == 0)
                throw input_error;

            index_t i = 0;
            for(; true; i = (i + 1 == std::size(devices_)) ? 0 : i + 1)
            {
                if (is_number(token))
                {
                    if (typeid(*(devices_[i])) != typeid(control_t))
                        throw input_error;
                    
                    index_t from = static_cast<index_t>(std::stoll(token));

                    token = read_token(stream);
                    if (token != "to")
                        throw input_error;

                    token = read_token(stream);
                    if (not is_number(token))
                        throw input_error;
                    index_t to = static_cast<index_t>(std::stoll(token));

                    token = read_token(stream);
                    if (token != ";")
                        throw input_error;

                    instruction_set_.push_back(control_operation(from, to));
                }

                else
                {
                    auto it = keywords.find(token);
                    if (it == std::end(keywords) or it->second < 8)
                        throw input_error;

                    switch (it->second)
                    {
                        case 8:
                        {
                            if (typeid(*(devices_[i])) != typeid(input_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(8, c));
                            break;
                        }

                        case 9:
                        {
                            if (typeid(*(devices_[i])) != typeid(input_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(9, c));
                            break;
                        }

                        case 10:
                        {
                            if (typeid(*(devices_[i])) != typeid(input_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(10, 0));
                            break;
                        }

                        case 11:
                        {
                            if (typeid(*(devices_[i])) != typeid(output_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(11, c));
                            break;
                        }

                        case 12:
                        {
                            if (typeid(*(devices_[i])) != typeid(stack_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(12, c));
                            break;
                        }

                        case 13:
                        {
                            if (typeid(*(devices_[i])) != typeid(stack_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(13, c));
                            break;
                        }

                        case 14:
                        {
                            if (typeid(*(devices_[i])) != typeid(stack_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(14, c));
                            break;
                        }

                        case 15:
                        {
                            if (typeid(*(devices_[i])) != typeid(stack_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(15, 0));
                            break;
                        }

                        case 16:
                        {
                            if (typeid(*(devices_[i])) != typeid(unsigned_counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(16, 0));
                            break;
                        }

                        case 17:
                        {
                            if (typeid(*(devices_[i])) != typeid(unsigned_counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(17, 0));
                            break;
                        }

                        case 18:
                        {
                            if (typeid(*(devices_[i])) != typeid(unsigned_counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(18, 0));
                            break;
                        }

                        case 19:
                        {
                            if (typeid(*(devices_[i])) != typeid(unsigned_counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(19, 0));
                            break;
                        }

                        case 20:
                        {
                            if (typeid(*(devices_[i])) != typeid(counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(20, 0));
                            break;
                        }

                        case 21:
                        {
                            if (typeid(*(devices_[i])) != typeid(counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(21, 0));
                            break;
                        }

                        case 22:
                        {
                            if (typeid(*(devices_[i])) != typeid(counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(22, 0));
                            break;
                        }

                        case 23:
                        {
                            if (typeid(*(devices_[i])) != typeid(counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(23, 0));
                            break;
                        }

                        case 24:
                        {
                            if (typeid(*(devices_[i])) != typeid(counter_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(24, 0));
                            break;
                        }

                        case 25:
                        {
                            if (typeid(*(devices_[i])) != typeid(tape_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(25, c));
                            break;
                        }

                        case 26:
                        {
                            if (typeid(*(devices_[i])) != typeid(tape_t))
                                throw input_error;

                            token = read_token(stream);
                            if (not is_number(token))
                                throw input_error;
                            character_t c = static_cast<character_t>(std::stoll(token));

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(26, c));
                            break;
                        }

                        case 27:
                        {
                            if (typeid(*(devices_[i])) != typeid(tape_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(27, 0));
                            break;
                        }

                        case 28:
                        {
                            if (typeid(*(devices_[i])) != typeid(tape_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(28, 0));
                            break;
                        }

                        case 29:
                        {
                            if (typeid(*(devices_[i])) != typeid(tape_t))
                                throw input_error;

                            token = read_token(stream);
                            if (token != ";")
                                throw input_error;

                            instruction_set_.push_back(operation(29, 0));
                            break;
                        }

                        default:
                            throw input_error;
                    } 
                }

                try
                {
                    token = read_token(stream);
                }
                catch (std::ios_base::failure &)
                {
                    *this = machine_t{std::move(devices_), std::move(instruction_set_),
                        std::move(alphabet_)};
                    return;
                }
            }

            *this = machine_t{std::move(devices_), std::move(instruction_set_),
                std::move(alphabet_)};

            return;
        }
        catch (std::ios_base::failure &)
        {
            throw input_error;
        }
    }

    machine_t &machine_t::load(std::istream &stream)
        { return *this = machine_t{stream}; }

    void machine_t::store(std::ostream &arg) const
    {
        std::ostringstream stream;
        static const std::runtime_error output_error{"In machine_t::store(std::ostream &)."};

        stream << alphabet_.n_characters() << ";\n";
        for (const auto &i : devices_)
        {
            if (typeid(*i) == typeid(control_t))
            {
                const control_t &control = dynamic_cast<const control_t &>(*i);
                const auto &terminating_states = control.terminator().terminating_states();

                std::multimap<string_t, index_t> map;
                for (const auto &i : terminating_states)
                    map.insert(std::pair<string_t, index_t>{i.second, i.first});

                const string_t *last_string = nullptr;

                stream << "control " << control.initialiser().initial_state() << " ";

                for (const auto &i : map)
                {
                    bool new_string;
                    if (not last_string)
                        new_string = true;
                    else
                        new_string = (i.first != *last_string);
                    last_string = &i.first;

                    if (new_string)
                        stream << ": " << last_string->to_ascii() << ", ";
                    else
                        stream << ", ";
                    stream << i.second;
                }
                stream << ";\n";
            }

            else if (typeid(*i) == typeid(input_t))
                stream << "input " << dynamic_cast<const input_t &>(*i).alphabet().n_characters()
                    << ";\n";

            else if (typeid(*i) == typeid(output_t))
                stream << "output " << dynamic_cast<const output_t &>(*i).alphabet().n_characters()
                    << ";\n";

            else if (typeid(*i) == typeid(stack_t))
            {
                const stack_t &stack = dynamic_cast<const stack_t &>(*i);
                const stack_initialiser_t &initialiser = stack.initialiser();
                const stack_terminator_t &terminator = stack.terminator();

                stream << "stack " << stack.alphabet().n_characters() << " ";

                if (typeid(initialiser) == typeid(stack_initialiser_empty_t))
                    stream << "empty ";
                else if (typeid(initialiser) == typeid(stack_initialiser_string_t))
                    stream << "string ";
                else
                    throw output_error;

                if (typeid(terminator) == typeid(stack_terminator_empty_t))
                    stream << "empty;\n";
                else if (typeid(terminator) == typeid(stack_terminator_string_t))
                    stream << "string;\n";
                else
                    throw output_error;
            }

            else if (typeid(*i) == typeid(unsigned_counter_t))
            {
                const unsigned_counter_t &unsigned_counter =
                    dynamic_cast<const unsigned_counter_t &>(*i);
                const unsigned_counter_initialiser_t &initialiser = unsigned_counter.initialiser();
                const unsigned_counter_terminator_t &terminator = unsigned_counter.terminator();

                stream << "ucounter ";

                if (typeid(initialiser) == typeid(unsigned_counter_initialiser_zero_t))
                    stream << "zero ";
                else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_ascii_t))
                    stream << "ascii ";
                else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_b_ary_t))
                    stream << "b_ary ";
                else if (typeid(initialiser) == typeid(unsigned_counter_initialiser_b_adic_t))
                    stream << "b_adic ";
                else
                    throw output_error;

                if (typeid(terminator) == typeid(unsigned_counter_terminator_zero_t))
                    stream << "zero;\n";
                else if (typeid(terminator) == typeid(unsigned_counter_terminator_string_t))
                    stream << "string;\n";
                else
                    throw output_error;
            }

            else if (typeid(*i) == typeid(counter_t))
            {
                const counter_t &counter = dynamic_cast<const counter_t &>(*i);
                const counter_initialiser_t &initialiser = counter.initialiser();
                const counter_terminator_t &terminator = counter.terminator();

                stream << "counter ";

                if (typeid(initialiser) == typeid(counter_initialiser_zero_t))
                    stream << "zero ";
                else if (typeid(initialiser) == typeid(counter_initialiser_ascii_t))
                    stream << "ascii ";
                else if (typeid(initialiser) == typeid(counter_initialiser_b_ary_t))
                    stream << "b_ary ";
                else if (typeid(initialiser) == typeid(counter_initialiser_b_adic_t))
                    stream << "b_adic ";
                else
                    throw output_error;

                if (typeid(terminator) == typeid(counter_terminator_zero_t))
                    stream << "zero;\n";
                else if (typeid(terminator) == typeid(counter_terminator_string_t))
                    stream << "string;\n";
                else
                    throw output_error;
            }

            else if (typeid(*i) == typeid(tape_t))
            {
                const tape_t &tape = dynamic_cast<const tape_t &>(*i);
                const tape_initialiser_t &initialiser = tape.initialiser();
                const tape_terminator_t &terminator = tape.terminator();

                stream << "tape " << tape.alphabet().n_characters() << " " <<
                    tape.default_character() << " ";

                if (typeid(initialiser) == typeid(tape_initialiser_empty_t))
                    stream << "empty ";
                else if (typeid(initialiser) == typeid(tape_initialiser_string_t))
                    stream << "string ";

                if (typeid(terminator) == typeid(tape_terminator_empty_t))
                    stream << "empty;\n";
                else if (typeid(terminator) == typeid(tape_terminator_string_t))
                    stream << "string;\n";
            }

            else
                throw output_error;
        }

        stream << '\n';

        index_t c = 0;
        for (const auto &i : instruction_set_)
        {
            constexpr index_t instruction_length = 20;
            std::ostringstream instruction;

            if (typeid(*i) == typeid(control_operation_t))
            {
                const control_operation_t &op =
                    dynamic_cast<const control_operation_t &>(*i);
                instruction << op.from() << " to " << op.to() << ';';
            }

            else if (typeid(*i) == typeid(input_operation_scan_t))
            {
                const input_operation_scan_t &op =
                    dynamic_cast<const input_operation_scan_t &>(*i);
                instruction << "scan " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(input_operation_next_t))
            {
                const input_operation_next_t &op =
                    dynamic_cast<const input_operation_next_t &>(*i);
                instruction << "next " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(input_operation_eof_t))
                instruction << "eof;";

            else if (typeid(*i) == typeid(output_operation_write_t))
            {
                const output_operation_write_t &op =
                    dynamic_cast<const output_operation_write_t &>(*i);
                instruction << "write " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(stack_operation_push_t))
            {
                const stack_operation_push_t &op =
                    dynamic_cast<const stack_operation_push_t &>(*i);
                instruction << "push " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(stack_operation_pop_t))
            {
                const stack_operation_pop_t &op =
                    dynamic_cast<const stack_operation_pop_t &>(*i);
                instruction << "pop " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(stack_operation_top_t))
            {
                const stack_operation_top_t &op =
                    dynamic_cast<const stack_operation_top_t &>(*i);
                instruction << "top " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(stack_operation_empty_t))
                instruction << "empty;";

            else if (typeid(*i) == typeid(unsigned_counter_operation_inc_t))
                instruction << "uinc;";

            else if (typeid(*i) == typeid(unsigned_counter_operation_dec_t))
                instruction << "udec;";

            else if (typeid(*i) == typeid(unsigned_counter_operation_zero_t))
                instruction << "uzero;";

            else if (typeid(*i) == typeid(unsigned_counter_operation_non_zero_t))
                instruction << "unonzero;";

            else if (typeid(*i) == typeid(counter_operation_inc_t))
                instruction << "inc;";

            else if (typeid(*i) == typeid(counter_operation_dec_t))
                instruction << "dec;";

            else if (typeid(*i) == typeid(counter_operation_zero_t))
                instruction << "zero;";

            else if (typeid(*i) == typeid(counter_operation_pos_t))
                instruction << "pos;";

            else if (typeid(*i) == typeid(counter_operation_neg_t))
                instruction << "neg;";

            else if (typeid(*i) == typeid(tape_operation_see_t))
            {
                const tape_operation_see_t &op =
                    dynamic_cast<const tape_operation_see_t &>(*i);
                instruction << "see " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(tape_operation_print_t))
            {
                const tape_operation_print_t &op =
                    dynamic_cast<const tape_operation_print_t &>(*i);
                instruction << "print " << op.character() << ';';
            }

            else if (typeid(*i) == typeid(tape_operation_move_l_t))
                instruction << "movel;";

            else if (typeid(*i) == typeid(tape_operation_move_r_t))
                instruction << "mover;";

            else if (typeid(*i) == typeid(tape_operation_athome_t))
                instruction << "athome;";

            else
                throw output_error;

            if (std::size(instruction.str()) > instruction_length)
                throw output_error; 

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

        arg << stream.str();
        return;
    }

    const alphabet_t &machine_t::alphabet() const { return alphabet_; }

    std::span<const string_t> machine_t::output() const
        { return {std::begin(output_), std::end(output_)}; }

    bool machine_t::deterministic() const { return deterministic_; }

    machine_t::machine_state_t machine_t::state() const { return state_; }

    void machine_t::initialise(const string_t &input)
    {
        if (input.alphabet() != alphabet_)
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

    noop_t *noop_t::clone() const { return new noop_t{}; }
    bool noop_t::applicable(const device_t &) const { return true; }
    void noop_t::apply(device_t &) const { return; }
    bool noop_t::correct_device(const device_t &) const { return true; }
    bool noop_t::intersecting_domain(const operation_t &) const { return true; }
    bool noop_t::intersecting_domain(const terminator_t &) const { return true; }

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

