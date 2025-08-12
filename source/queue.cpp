#include "queue.hpp"

namespace Machine
{
    queue_initialiser_empty_t *queue_initialiser_empty_t::clone() const
        { return new queue_initialiser_empty_t{*this}; }

    void queue_initialiser_empty_t::initialise(device_t &device, const std::string &) const
        { dynamic_cast<queue_t &>(device).queue().clear(); }

    queue_initialiser_string_t *queue_initialiser_string_t::clone() const
        { return new queue_initialiser_string_t{*this}; }

    void queue_initialiser_string_t::initialise(device_t &device, const std::string &string) const
    {
        string_t encoded_string = device.encoder()(string);
        std::deque<character_t> &queue = dynamic_cast<queue_t &>(device).queue();

        queue.clear();
        for (auto c : encoded_string)
            queue.push_front(c);
    }

    queue_terminator_empty_t *queue_terminator_empty_t::clone() const
        { return new queue_terminator_empty_t{*this}; }

    bool queue_terminator_empty_t::terminating(const device_t &device) const
        { return dynamic_cast<const queue_t &>(device).queue().empty(); }

    std::string queue_terminator_empty_t::terminate(const device_t &) const { return {}; }

    queue_terminator_string_t *queue_terminator_string_t::clone() const
        { return new queue_terminator_string_t{*this}; }

    bool queue_terminator_string_t::terminating(const device_t &) const
        { return true; }

    std::string queue_terminator_string_t::terminate(const device_t &device) const
    {
        const std::deque<character_t> &queue = dynamic_cast<const queue_t &>(device).queue();

        std::string ret;

        for (auto it = std::crbegin(queue); it != std::crend(queue); ++it)
            ret.push_back(device.encoder()(*it));

        return ret;
    }

    queue_t::queue_t(const queue_t &arg) :
        device_t{std::unique_ptr<encoder_t>{arg.encoder_->clone()}},
        queue_{arg.queue_},
        initialiser_{std::unique_ptr<queue_initialiser_t>{arg.initialiser_->clone()}},
        terminator_{std::unique_ptr<queue_terminator_t>{arg.terminator_->clone()}} {}

    queue_t &queue_t::operator=(const queue_t &arg) { return *this = queue_t{arg}; }

    queue_t::queue_t(std::unique_ptr<encoder_t> encoder,
            std::unique_ptr<queue_initialiser_t> initialiser,
            std::unique_ptr<queue_terminator_t> terminator) :
        device_t{std::move(encoder)},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)}
    {
        if (not encoder_)
            encoder_.reset(new encoder_ascii_t{});
    }

    queue_t *queue_t::clone() const { return new queue_t{*this}; }

    std::deque<character_t> &queue_t::queue() { return queue_; }
    const std::deque<character_t> &queue_t::queue() const { return queue_; }
    const queue_initialiser_t &queue_t::initialiser() const { return *initialiser_; }
    const queue_terminator_t &queue_t::terminator() const { return *terminator_; }
    std::string queue_t::print_state() const
    {
        std::string ret;
        for (character_t c : queue_)
            ret.push_back(encoder()(c));

        return ret;
    }

    bool queue_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(queue_t); }

    queue_operation_enqueue_t::queue_operation_enqueue_t(character_t character) :
        character_{character} {}

    queue_operation_enqueue_t *queue_operation_enqueue_t::clone() const
        { return new queue_operation_enqueue_t{*this}; }

    bool queue_operation_enqueue_t::applicable(const device_t &) const { return true; }

    void queue_operation_enqueue_t::apply(device_t &device) const
    {
        if (character_ > device.encoder().alphabet().max_character())
            throw std::runtime_error{"In Machine::queue_operation_enqueue_t::apply(device_t &) "
                "const:\nThe character to push excedes the maximum character.\n"};

        dynamic_cast<queue_t &>(device).queue().push_back(character_);
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
        const std::deque<character_t> &queue = dynamic_cast<const queue_t &>(device).queue();

        return (not std::empty(queue) and queue.front() == character_);
    }

    void queue_operation_dequeue_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        dynamic_cast<queue_t &>(device).queue().pop_front();
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

        throw std::runtime_error{"In Machine::queue_operation_dequeue_t::intersecting_domain("
            "const operation_t &):\nUnknown operation.\n"};
    }

    bool queue_operation_dequeue_t::intersecting_domain(const terminator_t &terminator) const
    {
        if (typeid(terminator) == typeid(queue_terminator_empty_t))
            return false;
        if (typeid(terminator) == typeid(queue_terminator_string_t))
            return true;

        throw std::runtime_error{"In Machine::queue_operation_dequeue_t::intersecting_domain("
            "const terminator_t &):\nUnknown terminator.\n"};
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

        throw std::runtime_error{"In Machine::queue_operation_empty_t::intersecting_domain("
            "const operation_t &):\nUnknown operation.\n"};
    }

    bool queue_operation_empty_t::intersecting_domain(const terminator_t &) const
        { return true; }
}

