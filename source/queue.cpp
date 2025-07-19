#include "queue.hpp"

namespace Machine
{
    void convert(std::queue<character_t> from, string_t &to)
    {
        to.clear();

        for (; not std::empty(from); from.pop())
            to.push(from.front());

        return;
    }

    void convert(const string_t &from, std::queue<character_t> &to)
    {
        to = std::queue<character_t>{};

        for (auto it = std::crbegin(from.data()); it != std::crend(from.data()); it++)
            to.push(*it);

        return;
    }

    queue_initialiser_empty_t *queue_initialiser_empty_t::clone() const
        { return new queue_initialiser_empty_t{*this}; }
    void queue_initialiser_empty_t::initialise(device_t &device, const string_t &) const
        { dynamic_cast<queue_t &>(device).queue() = std::queue<character_t>{}; }

    queue_initialiser_string_t *queue_initialiser_string_t::clone() const
        { return new queue_initialiser_string_t{*this}; }
    void queue_initialiser_string_t::initialise(device_t &device, const string_t &string) const
    {
        queue_t &queue = dynamic_cast<queue_t &>(device);
        if (queue.alphabet() != string.alphabet())
            throw std::runtime_error(
                    "In queue_initialiser_t::initialise(device_t &, cons string_t &)");

        convert(string, queue.queue());

        return;
    }

    queue_terminator_empty_t *queue_terminator_empty_t::clone() const
        { return new queue_terminator_empty_t{*this}; }
    bool queue_terminator_empty_t::terminating(const device_t &device) const
        { return dynamic_cast<const queue_t &>(device).queue().empty(); }
    string_t queue_terminator_empty_t::terminate(const device_t &device) const
        { return {dynamic_cast<const queue_t &>(device).alphabet()}; }

    queue_terminator_string_t *queue_terminator_string_t::clone() const
        { return new queue_terminator_string_t{*this}; }
    bool queue_terminator_string_t::terminating(const device_t &) const
        { return true; }
    string_t queue_terminator_string_t::terminate(const device_t &device) const
    {
        const queue_t &queue = dynamic_cast<const queue_t &>(device);

        string_t string{queue.alphabet()};
        convert(queue.queue(), string);

        return string;
    }

    queue_t::queue_t(const queue_t &arg) :
        queue_{arg.queue_},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()},
        alphabet_{arg.alphabet_} {}

    queue_t &queue_t::operator=(const queue_t &arg)
    {
        queue_ = arg.queue_;
        initialiser_.reset(arg.initialiser_->clone());
        terminator_.reset(arg.terminator_->clone());
        alphabet_ = arg.alphabet_;
        return *this;
    }

    queue_t::queue_t(const alphabet_t &alphabet,
            const queue_initialiser_t &initialiser, const queue_terminator_t &terminator) :
        initialiser_{initialiser.clone()},
        terminator_{terminator.clone()},
        alphabet_{alphabet} {}

    queue_t::queue_t(const alphabet_t &alphabet,
            std::unique_ptr<queue_initialiser_t> initialiser,
            std::unique_ptr<queue_terminator_t> terminator) :
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)},
        alphabet_{alphabet} {}

    queue_t *queue_t::clone() const { return new queue_t{*this}; }

    const alphabet_t &queue_t::alphabet() const { return alphabet_; }
    std::queue<character_t> &queue_t::queue() { return queue_; }
    const std::queue<character_t> &queue_t::queue() const { return queue_; }
    const queue_initialiser_t &queue_t::initialiser() const { return *initialiser_; }
    const queue_terminator_t &queue_t::terminator() const { return *terminator_; }

    bool queue_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(queue_t); }

    queue_operation_enqueue_t::queue_operation_enqueue_t(character_t character) :
        character_{character} {}
    queue_operation_enqueue_t *queue_operation_enqueue_t::clone() const
        { return new queue_operation_enqueue_t{*this}; }

    bool queue_operation_enqueue_t::applicable(const device_t &) const { return true; }
    void queue_operation_enqueue_t::apply(device_t &device) const
    {
        queue_t &queue = dynamic_cast<queue_t &>(device);
        if (character_ >= queue.alphabet().n_characters())
            throw std::runtime_error{"In queue_operation_enqueue_t::apply(device_t &) const."};
        queue.queue().push(character_);
        return;
    }

    bool queue_operation_enqueue_t::intersecting_domain(const operation_t &) const { return true; }
    bool queue_operation_enqueue_t::intersecting_domain(const terminator_t &) const { return true; }

    character_t queue_operation_enqueue_t::character() const { return character_; }

    queue_operation_dequeue_t::queue_operation_dequeue_t(character_t character) :
        character_{character} {}
    queue_operation_dequeue_t *queue_operation_dequeue_t::clone() const
        { return new queue_operation_dequeue_t{*this}; }

    bool queue_operation_dequeue_t::applicable(const device_t &device) const
    {
        const std::queue<character_t> &queue = dynamic_cast<const queue_t &>(device).queue();

        return (not std::empty(queue) and queue.front() == character_);
    }

    void queue_operation_dequeue_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        dynamic_cast<queue_t &>(device).queue().pop();
        return;
    }

    bool queue_operation_dequeue_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(queue_operation_enqueue_t))
            return true;
        if (typeid(operation) == typeid(queue_operation_dequeue_t))
            return character_ ==
                dynamic_cast<const queue_operation_dequeue_t &>(operation).character();
        if (typeid(operation) == typeid(queue_operation_empty_t))
            return false;
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error(
                "In queue_operation_dequeue_t::intersecting_domain(const operation_t &).");
    }

    bool queue_operation_dequeue_t::intersecting_domain(const terminator_t &terminator) const
    {
        if (typeid(terminator) == typeid(queue_terminator_empty_t))
            return false;
        if (typeid(terminator) == typeid(queue_terminator_string_t))
            return true;

        throw std::runtime_error(
                "In queue_operation_dequeue_t::intersecting_domain(const terminator_t &).");
    }

    character_t queue_operation_dequeue_t::character() const { return character_; }

    queue_operation_empty_t *queue_operation_empty_t::clone() const
        { return new queue_operation_empty_t{*this}; }

    bool queue_operation_empty_t::applicable(const device_t &device) const
        { return dynamic_cast<const queue_t &>(device).queue().empty(); }

    void queue_operation_empty_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);
        return;
    }

    bool queue_operation_empty_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(queue_operation_enqueue_t))
            return true;
        if (typeid(operation) == typeid(queue_operation_dequeue_t))
            return false;
        if (typeid(operation) == typeid(queue_operation_empty_t))
            return true;
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error(
                "In queue_operation_empty_t::intersecting_domain(const operation_t &).");
    }

    bool queue_operation_empty_t::intersecting_domain(const terminator_t &) const
        { return true; }
}

