#pragma once
#include "machine.hpp"

#include <queue>

namespace Machine
{
    class queue_t;
    class queue_operation_t;
    class queue_operation_enqueue_t;
    class queue_operation_dequeue_t;
    class queue_operation_empty_t;
    class queue_initialiser_t;
    class queue_initialiser_empty_t;
    class queue_initialiser_string_t;
    class queue_terminator_t;
    class queue_terminator_empty_t;
    class queue_terminator_string_t;

    class queue_initialiser_t : public initialiser_t
    {
    public:
        virtual ~queue_initialiser_t() = default;
        queue_initialiser_t *clone() const override = 0;
    };

    class queue_initialiser_empty_t final : public queue_initialiser_t
    {
    public:
        queue_initialiser_empty_t *clone() const override;
    public:
        virtual void initialise(device_t &, const string_t &) const override;
    };

    class queue_initialiser_string_t final : public queue_initialiser_t
    {
    public:
        queue_initialiser_string_t *clone() const override;
    public:
        virtual void initialise(device_t &, const string_t &) const override;
    };

    class queue_terminator_t : public terminator_t
    {
    public:
        virtual ~queue_terminator_t() = default;
        queue_terminator_t *clone() const override = 0;
    };

    class queue_terminator_empty_t final : public queue_terminator_t
    {
    public:
        queue_terminator_empty_t *clone() const override;
    public:
        bool terminating(const device_t &) const override;
        string_t terminate(const device_t &) const override;
    };

    class queue_terminator_string_t final : public queue_terminator_t
    {
    public:
        queue_terminator_string_t *clone() const override;
    public:
        bool terminating(const device_t &) const override;
        string_t terminate(const device_t &) const override;
    };

    class queue_t final : public device_t
    {
    private:
        std::queue<character_t> queue_{};
        std::unique_ptr<queue_initialiser_t> initialiser_;
        std::unique_ptr<queue_terminator_t> terminator_;
        alphabet_t alphabet_;

    public:
        queue_t() = delete;
        ~queue_t () = default;

        queue_t(const queue_t &);
        queue_t &operator=(const queue_t &);

        queue_t(queue_t &&) noexcept = default;
        queue_t &operator=(queue_t &&) noexcept = default;

        queue_t(const alphabet_t &, const queue_initialiser_t &, const queue_terminator_t &);
        queue_t(const alphabet_t &,
                std::unique_ptr<queue_initialiser_t>, std::unique_ptr<queue_terminator_t>);
        queue_t *clone() const override;
            
    public:
        const alphabet_t &alphabet() const;
        std::queue<character_t> &queue();
        const std::queue<character_t> &queue() const;
        const queue_initialiser_t &initialiser() const override;
        const queue_terminator_t &terminator() const override;
    };

    class queue_operation_t : public operation_t
    {
    public:
        virtual ~queue_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
    };

    class queue_operation_enqueue_t final: public queue_operation_t
    {
    private:
        character_t character_;

    public:
        queue_operation_enqueue_t(character_t);
        queue_operation_enqueue_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class queue_operation_dequeue_t final: public queue_operation_t
    {
    private:
        character_t character_;

    public:
        queue_operation_dequeue_t(character_t);
        queue_operation_dequeue_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class queue_operation_empty_t final: public queue_operation_t
    {
    public:
        queue_operation_empty_t() = default;
        queue_operation_empty_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };
}

