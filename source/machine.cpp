#include "machine.hpp"
#include "control.hpp"

#include <algorithm>
#include <numeric>

namespace Machine
{
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

    machine_t::machine_t(std::vector<std::unique_ptr<device_t>> devices,
            std::vector<std::shared_ptr<operation_t>> instruction_set) :
        devices_{std::move(devices)}, instruction_set_{std::move(instruction_set)}
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
                    throw invalid_operation{**k, **j};

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

    std::span<const string_t> machine_t::output() const
        { return {std::begin(output_), std::end(output_)}; }

    bool machine_t::deterministic() const { return deterministic_; }

    machine_t::machine_state_t machine_t::state() const { return state_; }

    void machine_t::initialise(const string_t &input)
    {
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

    invalid_operation::invalid_operation(const device_t &device, const operation_t &operation) :
        device_(device), operation_(operation) {}

    const device_t &invalid_operation::device() const { return device_; }

    const operation_t &invalid_operation::operation() const { return operation_; }

    invalid_terminator::invalid_terminator(const terminator_t &terminator, const device_t &device)
        : terminator_{terminator}, device_{device} {}

    const terminator_t &invalid_terminator::terminator() const { return terminator_; }

    const device_t &invalid_terminator::device() const { return device_; }
}

