#include "console.hpp"
#include "tape.hpp"

#include <algorithm>
#include <cmath>
#include <cassert>

namespace Machine
{
    static ftxui::Component resizable_row(ftxui::Components &arg, std::span<int> dimensions)
    {
        using namespace ftxui;

        if (std::empty(arg) or std::size(arg) > std::size(dimensions) + 1)
            abort("resizable_row(ftxui::Components &, std::span<int>)");
        
        Component ret = arg.back();

        auto i = std::rbegin(arg) + 1;
        auto j = std::begin(dimensions) + std::size(arg) - 2;
        for(; i != std::rend(arg); ++i, --j)
            ret = ResizableSplitLeft(*i, ret, &*j);

        ret = Renderer(ret, [ret] { return ret->Render(); });

        return ret;
    }

    static ftxui::Component resizable_table(std::vector<ftxui::Components> &arg,
        std::span<int> dimensions)
    {
        using namespace ftxui;

        if (std::empty(arg))
            abort("resizable_table(std::vector<ftxui::Components> &, std::span<int>)");

        Components rows;
        
        for (auto &i : arg)
        {
            rows.emplace_back(resizable_row(i, dimensions));
            rows.emplace_back(Renderer([]{ return separator(); }));
        }
        
        return Container::Vertical(rows);
    }

    static ftxui::Component scroll_table(ftxui::Components &header,
        std::vector<ftxui::Components> &rows,
        ftxui::Component &slider_x, ftxui::Component &slider_y,
        std::span<int> horizontal_sizes, float &scroll_x, float &scroll_y)
    {
        using namespace ftxui;

        Component header_row = resizable_row(header, horizontal_sizes);
        header_row = Renderer(header_row, [header_row, &scroll_x]
            { return header_row->Render() | focusPositionRelative(scroll_x, 0)
                | frame | flex; }) | size(HEIGHT, EQUAL, 1);

        Component table = resizable_table(rows, horizontal_sizes);

        table = Renderer(table, [table, &scroll_x, &scroll_y]
            { return table->Render() | focusPositionRelative(scroll_x, scroll_y)
                | frame | flex; });

        Component ret =
        Container::Vertical({
            Container::Horizontal({
                Container::Vertical({
                    header_row,
                    Renderer([] { return separatorDouble(); }),
                    table | yflex_grow
                }) | xflex,
                slider_y
            }) | yflex,
            slider_x
        });
                
        return ret;
    }

    static void replace_child(ftxui::Component &root, ftxui::Component &obsolete_child,
        ftxui::Component &new_child)
    {
        using namespace ftxui;

        Component parent{nullptr};

        for (Component i = root; not parent; i = i->ChildAt(0))
        {
            if (not i or i->ChildCount() != 1)
                abort("replace_child(ftxui::Component &, ftxui::Component &, ftxui::Component &)");
            
            if (i->ChildAt(0).get() == obsolete_child.get())
                parent = i;
        }

        parent->DetachAllChildren();
        parent->Add(new_child);
        new_child = parent->ChildAt(0);

        return;
    }

    console_t::tui_t::devices_t::devices_t(console_t &console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_x_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_y_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Down,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | bgcolor(ftxui::Color::GrayLight)}
    {
        using namespace ftxui;

        component_ = Renderer(child_component_, [this]
        {
            if (update_)
            {
                update_ = false;

                std::unique_lock<std::mutex> &l = console_.tui().lock_;

                if (not l)
                   l.lock(); 

                Component old_child = child_component_;

                dimensions_ = {6, 5, 6, 5};
                padding_ = 16;

                std::shared_ptr<machine_t> focus = *console_.focus();

                Component top_left_cell = Renderer(
                    [s = std::string("Device"), &d = dimensions_[0]] mutable
                    {
                        s.resize(d, ' ');
                        return text(s);
                    });

                Components top_row{
                    Renderer([s = std::string("Input")] { return text(s); }),
                    Renderer([s = std::string("Output")] { return text(s); }),
                    Renderer([s = std::string("State")] { return text(s); }),
                    Renderer(
                        [&p = padding_, s = std::string("Next instruction")] mutable
                        {
                            s.resize(p, ' ');
                            return text(s);
                        })
                };

                Component scroll_top_row = resizable_row(top_row,
                        std::span<int>{std::begin(dimensions_) + 1, std::end(dimensions_)});

                scroll_top_row = Renderer(scroll_top_row, [r = scroll_top_row, &sx = scroll_x_]
                    { return r->Render() | focusPositionRelative(sx, 0) | frame | flex; }
                );

                if (not focus)
                    child_component_ = Container::Vertical({
                        Container::Horizontal({
                            top_left_cell,
                            Renderer([] { return separatorDouble(); }),
                            scroll_top_row
                        }),
                        Renderer([] { return separatorDouble(); }),
                        Renderer([] { return text("Uninitialised console") | center; })
                    });

                else
                {
                    Components left_column;
                    std::vector<Components> rows;

                    const std::vector<std::unique_ptr<device_t>> &devices = focus->devices();
                    index_t n = std::size(devices);

                    for (index_t i = 0; i != n; ++i)
                    {
                        std::string device_name = devices[i]->print_name();

                        if (typeid(*devices[i]) != typeid(tape_t))
                            left_column.emplace_back(
                                Renderer([s = std::move(device_name), &d = dimensions_[0]] mutable
                                    {
                                        d = std::max(d, static_cast<int>(std::size(s)));
                                        s.resize(d, ' ');
                                        return text(s);
                                    }));

                        else
                            left_column.emplace_back(Container::Vertical({
                                Renderer([s = std::move(device_name), &d = dimensions_[0]] mutable
                                    {
                                        d = std::max(d, static_cast<int>(std::size(s)));
                                        s.resize(d, ' ');
                                        return text(s);
                                    }),
                                Renderer([] { return text(""); })}));

                        left_column.emplace_back(Renderer([]{ return separator(); }));

                        rows.emplace_back();

                        rows.back().emplace_back(Renderer(
                            [s = input_strings_[i], &d = dimensions_[1]]
                            {
                                d = std::max(d, static_cast<int>(std::size(*s)));
                                return text(*s) | xflex_grow;
                            }));

                        rows.back().emplace_back(Renderer(
                            [&c = console_, &l, i, &d = dimensions_[2]]
                            {
                                std::string s;
                                if (not l or not *c.focus())
                                    s = "Unavailable";
                                else
                                    s = (*c.focus())->output()[i];

                                d = std::max(d, static_cast<int>(std::size(s)));

                                return text(std::move(s));
                            }));

                        if (typeid(*devices[i]) != typeid(tape_t))
                            rows.back().emplace_back(Renderer(
                                [&c = console_, &l, i, &d = dimensions_[3]]
                                {
                                    std::string s;
                                    
                                    if (not l or not *c.focus())
                                        s = "Unavailable";
                                    else
                                    {
                                        machine_t &m = **c.focus();
                                        machine_t::machine_state_t state = m.state();
                                        
                                        if (state == machine_t::machine_state_t::invalid)
                                            s = "Invalid";
                                        else if (state == machine_t::machine_state_t::halted)
                                            s = "Halted";
                                        else
                                            s = m.devices()[i]->print_state();
                                    }

                                    d = std::max(d, static_cast<int>(std::size(s)));
                                    return text(std::move(s));
                                }));

                        else
                            rows.back().emplace_back(Container::Vertical({
                                Renderer([&c = console_, &l, i, &d = dimensions_[3]]
                                {
                                    std::string s;

                                    if (not l or not *c.focus())
                                        s = "Unavailable";
                                    else
                                    {
                                        machine_t &m = **c.focus();
                                        machine_t::machine_state_t state = m.state();
                                    
                                        if (state == machine_t::machine_state_t::invalid)
                                            s = "Invalid";
                                        else if (state == machine_t::machine_state_t::halted)
                                            s = "Halted";
                                        else
                                            s = m.devices()[i]->print_state();
                                    }
                                    
                                    d = std::max(d, static_cast<int>(std::size(s)));
                                    return text(std::move(s));
                                }),
                                Renderer([&c = console_, &l, i]
                                {
                                    std::string s;

                                    if (l and *c.focus())
                                    {
                                        machine_t &m = **c.focus();
                                        machine_t::machine_state_t state = m.state();

                                        if (state != machine_t::machine_state_t::invalid and
                                            state != machine_t::machine_state_t::halted)
                                        {
                                            const tape_t &tape = dynamic_cast<const tape_t &>(
                                                    *m.devices()[i]);
                                            s.resize(tape.string().pos(), ' ');
                                            s.push_back('^');
                                        }
                                    }

                                    return text(std::move(s));
                                })}));

                        rows.back().emplace_back(
                            Renderer([&c = console_, &l, i, &p = padding_]
                                {
                                    std::string s;
                                    
                                    if (not l or not *c.focus())
                                        s = "Unavailable";
                                    else
                                        s = (**c.focus()).next_instruction(i);

                                    p = std::max(p, std::size(s));
                                    s.resize(p, ' ');
                                    return text(std::move(s));
                                })
                        );
                    }


                    Component table = resizable_table(rows,
                            std::span<int>{std::begin(dimensions_) + 1, std::end(dimensions_)});

                    table = Renderer(table, [table, &sx = scroll_x_, &sy = scroll_y_]
                        {
                            return table->Render() | focusPositionRelative(sx, sy) | frame
                            | flex;
                        });

                    Component scroll_left_column = Container::Vertical(left_column);

                    scroll_left_column = Renderer(scroll_left_column,
                        [scroll_left_column, &sy = scroll_y_, n = std::size(left_column)]
                        {
                            return scroll_left_column->Render() | focusPositionRelative(0, sy)
                            | frame;
                        });

                    child_component_ = Container::Vertical({
                        Container::Horizontal({
                            Container::Vertical({
                                Container::Horizontal({
                                    top_left_cell,
                                    Renderer([] { return separatorDouble(); }),
                                    scroll_top_row
                                }),
                                Renderer([]{ return separatorDouble(); }),
                                Container::Horizontal({
                                    scroll_left_column,
                                    Renderer([] { return separatorDouble(); }),
                                    table
                                })
                            }) | xflex,
                            slider_y_
                        }) | yflex,
                        slider_x_
                    });
                }

                replace_child(component_, old_child, child_component_);
            }

            return child_component_->Render();
        });

    component_ |= size(WIDTH, EQUAL, console_.tui().width(0));
    component_ |= size(HEIGHT, EQUAL, console_.tui().height(0));
    component_ |= borderHeavy;

    return;
}

    void console_t::tui_t::devices_t::update() { update_ = true; }

    std::vector<std::shared_ptr<std::string>> &console_t::tui_t::devices_t::input_strings()
        { return input_strings_; }

    ftxui::Component &console_t::tui_t::devices_t::component() { return component_; }

    console_t::tui_t::summary_t::summary_t(console_t &console) : console_{console}
    {
        using namespace ftxui;
         
        std::vector<Components> table;
        table.emplace_back(
            Components{
                Renderer([] { return text("State") | align_right; }),
                Renderer([] { return separator(); }),
                Renderer([] { return text("Deterministic") | align_right; }),
                Renderer([] { return separator(); }),
                Renderer([] { return text("Global instruction counter") | align_right; }), 
                Renderer([] { return separator(); }),
                Renderer([] { return text("Local instruction counter") | align_right; }),
            });
        table.emplace_back(
            Components{
                Renderer([&c = console_.state_]
                    {
                        index_t a = static_cast<index_t>(c);
                        std::string s;
                        if (a & console_t::console_state_t::running)
                            s = "Running";
                        else if (a & console_t::console_state_t::program_loaded)
                            s = "Program loaded";
                        else 
                            s = "Empty";

                        return text(std::move(s));
                    }),
                Renderer([] { return separator(); }),
                Renderer([&c = console_.state_]
                    {
                        std::string s;
                        if (c == console_t::console_state_t::empty)
                            s = "Not applicable";
                        else if (c & console_t::console_state_t::deterministic)
                            s = "Yes";
                        else
                            s = "No";

                        return text(std::move(s));
                    }),
                Renderer([] { return separator(); }),
                Renderer([&c = console_.instruction_counter_]
                    {
                        return text(std::to_string(c));
                    }),
                Renderer([] { return separator(); }),
                Renderer([&c = console_, &l = console_.tui().lock_]
                    {
                        std::string s;
                        if (l and *c.focus())
                            s = std::to_string((**c.focus()).computation_size());
                        else
                            s = "Not applicable";

                        return text(std::move(s));
                    })
            });
               
        component_ = Container::Vertical({
            Renderer([] { return text("Summary") | center; }),
            Renderer([] { return separatorDouble(); }),
            Container::Horizontal({
                Container::Vertical(table[0]) | xflex_grow,
                Renderer([]{ return separator(); }),
                Container::Vertical(table[1]) | xflex_grow
            }) | yflex_grow
        });

        component_ |= size(WIDTH, EQUAL, console_.tui().width(3));
        component_ |= size(HEIGHT, EQUAL, console_.tui().height(2));
        component_ |= borderHeavy;

        return;
    }

    ftxui::Component &console_t::tui_t::summary_t::component() { return component_; }

    console_t::tui_t::applicable_instructions_t::applicable_instructions_t(console_t &console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_x_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_y_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Down,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)}
    {
        using namespace ftxui;

        component_ = Renderer(child_component_, [this]
        {
            std::unique_lock<std::mutex> &l = console_.tui().lock_;

            index_t previous_selected = selected_;

            if (update_)
            {
                Component old_child = child_component_;

                bool_vector_.clear();

                previous_selected = negative_1;

                if (not l or not *console_.focus())
                    child_component_ = Container::Vertical({
                        Renderer([]{ return text("Available Instructions") | center; }),
                        Renderer([]{ return separatorDouble(); }),
                        Renderer([]{ return text("Unavailable") | center; } )
                    });

                else
                {
                    update_ = false;

                    std::shared_ptr<machine_t> focus = *console_.focus();

                    const std::vector<std::unique_ptr<device_t>> &devices = focus->devices();
                    index_t n = std::size(devices);

                    std::vector<std::string> applicable_instructions =
                        focus->print_applicable_instructions();

                    index_t s = std::size(applicable_instructions) / n;

                    bool_vector_.resize(s, bool_struct{false});

                    padding_ = std::size(devices.back()->print_name());

                    for (auto i = std::cbegin(applicable_instructions);
                            i != std::cend(applicable_instructions); i += n)
                        padding_ = std::max(padding_, std::size(*(i + n - 1)));

                    if (std::size(dimensions_) != n + 1)
                    {
                        dimensions_.resize(n + 1);
                        dimensions_.front() = 4;
                        std::fill(std::begin(dimensions_) + 1, std::end(dimensions_),
                            (console_.tui().width(3) - 4) / n);
                    }

                    Components header{Renderer([]{ return text(""); })};

                    for (auto i = std::begin(devices); i != std::end(devices) - 1; ++i)
                        header.emplace_back(Renderer([s = (*i)->print_name()]
                            { return text(s); }));

                    {
                        std::string s = devices.back()->print_name();
                        s.resize(padding_, ' ');
                        header.emplace_back(Renderer([s = std::move(s)] { return text(s); }));
                    }

                    Component header_row = resizable_row(header, dimensions_);
                    header_row = Renderer(header_row, [header_row, &sx = scroll_x_]
                        { return header_row->Render() | focusPositionRelative(sx, 0) |
                            frame | flex; });

                    if (std::empty(applicable_instructions))
                        child_component_ =
                            Container::Vertical({
                                Renderer([]{ return text("Applicable Instructions") | center; }),
                                Renderer([]{ return separatorDouble(); }),
                                Renderer([]
                                    { return text("No instruction is applicable") | center; })
                            });

                    else
                    {
                        std::vector<Components> rows;
                        index_t c = 0;
                        for (auto i = std::cbegin(applicable_instructions);
                                i != std::cend(applicable_instructions); i += n, ++c)
                        {
                            rows.emplace_back(Components{
                                Checkbox("", &(bool_vector_[c].bool_)) | center});

                            for (auto j = i; j != i + n - 1; ++ j)
                                rows.back().emplace_back(Renderer([s = *j]{ return text(s);}));
                            std::string s = *(i + n - 1);
                            s.resize(padding_, ' ');
                            rows.back().emplace_back(Renderer([s = std::move(s)]
                                { return text(s); }));
                        }

                        child_component_ =
                            Container::Vertical({
                                Renderer([]{ return text("Applicable Instructions") | center; }),
                                Renderer([]{ return separatorDouble(); }),
                                scroll_table(header, rows, slider_x_, slider_y_,
                                    std::span<int>(std::begin(dimensions_), std::end(dimensions_)),
                                    scroll_x_, scroll_y_) | yflex_grow,
                            });
                    }
                }

                replace_child(component_, old_child, child_component_);
            }

            for (index_t i = 0; i != std::size(bool_vector_); ++i)
            {
                if (i == selected_)
                    continue;
                if (bool_vector_[i].bool_)
                {
                    selected_ = i;
                    break;
                }
            }

            for (index_t i = 0; i != std::size(bool_vector_); ++i)
                bool_vector_[i].bool_ = i == selected_;

            if (not std::empty(bool_vector_) and previous_selected != selected_)
            {
                if (not l)
                    l.lock();

                if (*console_.focus())
                {
                    machine_t &m = **console_.focus();
                    if (selected_ < m.n_applicable_instructions())
                        m.select_instruction(selected_);
                }
            }

            return child_component_->Render();
        });

        component_ |= size(WIDTH, EQUAL, console_.tui().width(1));
        component_ |= size(HEIGHT, EQUAL, console_.tui().height(0));
        component_ |= yflex_grow;
        component_ |= borderHeavy;

        return;
    }
    
    void console_t::tui_t::applicable_instructions_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::applicable_instructions_t::component()
        { return component_; }

    console_t::tui_t::menu_t::menu_t(console_t &console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_x_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_y_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Down,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)}
    {
        using namespace ftxui;

        button_style_.transform = [](const ftxui::EntryState &state)
        {
            auto element = text(state.label) | borderLight | color(Color::Black);

            if (state.focused)
                element |= bgcolor(Color::GrayLight);
            else
                element |= bgcolor(Color::White); 

            return element;
        };

        input_style_.transform = [](InputState state)
        {
            state.element |= borderRounded;
            if (state.focused)
                state.element |= bgcolor(Color::GrayLight);
            else
                state.element |= bgcolor(Color::White);
            state.element |= color(Color::Black);

            return state.element;
        };

        strings_.emplace_back(std::make_shared<std::string>());
        strings_.emplace_back(console_.strings_[0]);
        strings_.emplace_back(console_.strings_[1]);

        component_ = Renderer(child_component_, [this]
            {
                if (update_)
                {
                    update_ = false;

                    std::unique_lock<std::mutex> &l = console_.tui().lock_;

                    if (not l)
                        l.lock();

                    Component old_child = child_component_;

                    ptr_t focus = *console_.focus();

                    strings_[0]->clear();

                    bool empty = console_.state_ == console_t::console_state_t::empty;

                    Components initialisers;

                    if (empty or not focus)
                       initialisers.emplace_back(Renderer([]
                            { return text("Empty console") | center; })); 

                    else
                    {
                        for (index_t i = 0; i != std::size(focus->devices()); ++i)
                            initialisers.emplace_back(
                                Input(strings_[i + 3].get(),
                                    focus->devices()[i]->print_name(), input_style_));

                        initialisers.emplace_back(
                            Button("Initialise", [&c = console_]
                                { c.initialise_individually(); }, button_style_));
                    }


                    Component step_button =
                        Button("Step", [&c = console_] { c.step(); }, button_style_);
                    Component run_button =
                        Button("Run", [&c = console_] { c.run(); }, button_style_);
                    Component step_all_button =
                        Button("Step all",
                                [&c = console_] { c.step_all(); }, button_style_);
                    Component run_all_button =
                        Button("Run all",
                                [&c = console_] { c.run_all(); }, button_style_);
                    Component pause_button =
                        Button("Pause", [&c = console_] { c.pause(); }, button_style_);
                    Component pause_halt_button =
                        Button("Pause when halt", [&c = console_, &p = pause_halt_]
                            { c.pause_halt() = not c.pause_halt(); }, button_style_);

                    Component buttons = Container::Vertical({
                        Container::Horizontal({
                            Renderer(step_button,
                                [&c = console_, &l, s = step_button, &t = console_.tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not l or not *c.focus() or (**c.focus()).state() !=
                                            machine_t::machine_state_t::running)
                                        return ret | inverted;
                                    else
                                        return ret;
                                }),
                            Renderer(run_button,
                                [&c = console_, &l, s = run_button, &t = console_.tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not l or not *c.focus() or (**c.focus()).state() !=
                                            machine_t::machine_state_t::running)
                                        return ret | inverted;
                                    else
                                        return ret;
                                })
                            }),
                        Container::Horizontal({
                            Renderer(step_all_button,
                                [&c = console_, &l, s = step_all_button, &t = console_.tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not l or not (c.state_ & console_state_t::running))
                                        return ret | inverted;
                                    else
                                        return ret;
                                }),
                            Renderer(run_all_button,
                                [&c = console_, &l, s = run_all_button, &t = console_.tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not l or not (c.state_ & console_state_t::running))
                                        return ret | inverted;
                                    else
                                        return ret;
                                })
                            }),
                        Container::Horizontal({
                            Renderer(pause_button, [s = pause_button, &t = console_.tui()]
                                { return s->Render() | size(WIDTH, EQUAL, (t.width(2) - 3) / 2); }),
                            Renderer(pause_halt_button, [&b = console_.pause_halt(),
                                s = pause_halt_button, &t = console_.tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not b)
                                        return ret | inverted;
                                    else
                                        return ret;
                                })
                            })
                        });

                    Components rows;
                    rows.emplace_back(Renderer(buttons, [buttons, &t = console_.tui()]
                        { return hbox({buttons->Render()}) | size(WIDTH, EQUAL, t.width(2) - 3); }
                    ));

                    rows.emplace_back(Renderer([]{ return separatorDouble(); }));
                    rows.emplace_back(Toggle(&tab_entries_, &tab_selected_));
                    rows.emplace_back(Renderer([]{ return separatorDouble(); }));

                    Component a;
                    if (empty)
                        a = Renderer([]{ return text("Empty console") | center; });
                    else
                        a = Container::Vertical({
                                Input(strings_[2].get(), "Input string", input_style_),
                                Button("Initialise", [&c = console_] { c.initialise_all(); },
                                    button_style_)
                            });

                    Component b = Container::Vertical(initialisers);

                    Component c = Container::Vertical({
                                Input(strings_[1].get(), "Enter the program name", input_style_),
                                Button("Load program", [&c = console_]
                                    { c.load_program(); }, button_style_)
                            });

                    rows.emplace_back(Container::Tab({
                        Renderer(a, [a, &t = console_.tui()]
                            { return hbox({a->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }),
                        Renderer(b, [b, &t = console_.tui()]
                            { return hbox({b->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }),
                        Renderer(c, [c, &t = console_.tui()]
                            { return hbox({c->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }),
                        }, &tab_selected_
                    ));

                    rows.emplace_back(Renderer([]{ return separatorDouble(); }));

                    Component d = Button("Quit", [&c = console_] {c.close_tui(); }, button_style_);

                    rows.emplace_back(Renderer(d, [d, &t = console_.tui()]
                        { return hbox({d->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }));

                    Component container = Container::Vertical(rows);
                    Component table = Renderer(container,
                        [container, &sx = scroll_x_, &sy = scroll_y_]
                            {
                                return container->Render() | focusPositionRelative(sx, sy) |
                                frame | flex;
                            });

                    child_component_ =
                        Container::Vertical({
                            Container::Horizontal({
                                Container::Vertical({
                                    Renderer([] { return text("Menu") | center;}),
                                    Renderer([]{ return separatorDouble(); }),
                                    Renderer([s = strings_[0]]
                                            { return text(*s) | border |
                                                bgcolor(Color::GrayLight); }),
                                    Button("Clear", [s = strings_[0]]
                                            { s->clear(); }, button_style_),
                                    Renderer([]{ return separatorDouble(); }),
                                    table | yflex_grow
                                }) | xflex,
                                slider_y_
                            }) | yflex,
                        slider_x_
                    });

                    
                    replace_child(component_, old_child, child_component_);
                }

                return child_component_->Render();
            });

        component_ |= size(WIDTH, EQUAL, console_.tui().width(2));
        component_ |= size(HEIGHT, EQUAL, console_.tui().height(1));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::menu_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::menu_t::component() { return component_; }

    int &console_t::tui_t::menu_t::tab_selected() { return tab_selected_; }

    std::vector<std::shared_ptr<std::string>> &console_t::tui_t::menu_t::strings()
        { return strings_; }

    console_t::tui_t::machines_t::machines_t(console_t &console) :
        console_{console}
    {
        using namespace ftxui;

        InputOption input_style{ftxui::InputOption::Default()};

        input_style.transform = [](InputState state)
        {
            if (state.is_placeholder)
                state.element |= color(Color::GrayDark);
            else
                state.element |= color(Color::Black);

            state.element |= bgcolor(Color::GrayLight);
            state.element |= align_right;

            return state.element;
        };

        std::vector<Components> table;
        table.emplace_back(Components{
            Renderer([] { return text("State") | center; }),
            Renderer([] { return separatorDouble(); }),
            Renderer([] { return text("Invalid") | align_right; }),
            Renderer([] { return separator(); }),
            Renderer([] { return text("Running") | align_right; }),
            Renderer([] { return separator(); }),
            Renderer([] { return text("Blocked") | align_right; }),
            Renderer([] { return separator(); }),
            Renderer([] { return text("Halted") | align_right; })
        });

        table.emplace_back(Components{
            Renderer([this] { return text("Count") | center; }),
            Renderer([] { return separatorDouble(); }),
            Renderer([this]
                {
                    std::string s;
                    if (not lock_)
                        s = "--";
                    else
                        s = std::to_string(std::size(console_.invalid_machines_));
                    return text(s);
                }),
            Renderer([] { return separator(); }),
            Renderer([this]
                {
                    std::string s;
                    if (not lock_)
                        s = "--";
                    else
                        s = std::to_string(std::size(console_.running_machines_));
                    return text(s);
                }),
            Renderer([] { return separator(); }),
            Renderer([this]
                {
                    std::string s;
                    if (not lock_)
                        s = "--";
                    else
                        s = std::to_string(std::size(console_.blocked_machines_));
                    return text(s);
                }),
            Renderer([] { return separator(); }),
            Renderer([this]
                {
                    std::string s;
                    if (not lock_)
                        s = "--";
                    else
                    {
                        index_t c = 0;
                        for (const auto &i : console_.halted_machines_)
                            c += std::size(i.second);
                        s = std::to_string(c);
                    }

                    return text(s);
                })
        });

        Component invalid_buttons = Container::Horizontal({
            Button("Prev", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::invalid, negative_2);

                return;
            }, ButtonOption::Ascii()),
            Button("Next", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::invalid, negative_1);

                return;
            }, ButtonOption::Ascii()),
            Input(&strings_[0], "Number", input_style) | xflex_grow,
            Button("Load", [this]{
                index_t c;
                try { c = std::stoll(strings_[0]); }
                catch (std::invalid_argument &)
                {
                    *console_.tui().menu().strings()[0] = "Error reading the number";
                    return;
                }

                if (not lock_)
                    lock_.lock();

                console_.change_focus(machine_t::machine_state_t::invalid, c);

                return;
            }, ButtonOption::Ascii()),
        });

        Component running_buttons = Container::Horizontal({
            Button("Prev", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::running, negative_2);

                return;
            }, ButtonOption::Ascii()),
            Button("Next", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::running, negative_1);

                return;
            }, ButtonOption::Ascii()),
            Input(&strings_[1], "Number", input_style) | xflex_grow,
            Button("Load", [this]{
                index_t c;
                try { c = std::stoll(strings_[1]); }
                catch (std::invalid_argument &)
                {
                    *console_.tui().menu().strings()[0] = "Error reading the number";
                    return;
                }

                if (not lock_)
                    lock_.lock();

                console_.change_focus(machine_t::machine_state_t::running, c);

                return;
            }, ButtonOption::Ascii()),
        });

        Component blocked_buttons = Container::Horizontal({
            Button("Prev", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::blocked, negative_2);

                return;
            }, ButtonOption::Ascii()),
            Button("Next", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::blocked, negative_1);

                return;
            }, ButtonOption::Ascii()),
            Input(&strings_[2], "Number", input_style) | xflex_grow,
            Button("Load", [this]{
                index_t c;
                try { c = std::stoll(strings_[2]); }
                catch (std::invalid_argument &)
                {
                    *console_.tui().menu().strings()[0] = "Error reading the number";
                    return;
                }

                if (not lock_)
                    lock_.lock();

                console_.change_focus(machine_t::machine_state_t::blocked, c);

                return;
            }, ButtonOption::Ascii()),
        });

        Component halted_buttons = Renderer([] { return text(""); });

        Component classes_buttons = Container::Horizontal({
            Button("Prev", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::halted, negative_2);

                return;
            }, ButtonOption::Ascii()),
            Button("Next", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus(machine_t::machine_state_t::halted, negative_1);

                return;
            }, ButtonOption::Ascii()),
            Input(&strings_[4], "Number", input_style) | xflex_grow,
            Button("Load", [this]{
                index_t c;
                try { c = std::stoll(strings_[4]); }
                catch (std::invalid_argument &)
                {
                    *console_.tui().menu().strings()[0] = "Error reading the number";
                    return;
                }

                if (not lock_)
                    lock_.lock();

                console_.change_focus(machine_t::machine_state_t::halted, c);

                return;
            }, ButtonOption::Ascii()),
        });

        Component class_buttons = Container::Horizontal({
            Button("Prev", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus_class(negative_2);

                return;
            }, ButtonOption::Ascii()),
            Button("Next", [this]{
                if (not lock_)
                    lock_.lock();
                console_.change_focus_class(negative_1);

                return;
            }, ButtonOption::Ascii()),
            Input(&strings_[5], "Number", input_style) | xflex_grow,
            Button("Load", [this]{
                index_t c;
                try { c = std::stoll(strings_[5]); }
                catch (std::invalid_argument &)
                {
                    *console_.tui().menu().strings()[0] = "Error reading the number";
                    return;
                }

                if (not lock_)
                    lock_.lock();

                console_.change_focus_class(c);

                return;
            }, ButtonOption::Ascii()),
        });

        table.emplace_back(Components{
            Renderer([] { return text("Focus") | center; }),
            Renderer([] { return separatorDouble(); }),
            invalid_buttons,
            Renderer([] { return separator(); }),
            running_buttons,
            Renderer([] { return separator(); }),
            blocked_buttons,
            Renderer([] { return separator(); }),
            halted_buttons
        });

        table.emplace_back(Components{
            Renderer([] { return text("Output classes") | center; }),
            Renderer([] { return separatorDouble(); }),
            Renderer([] { return text("Classes") | align_right; }),
            Renderer([] { return separator(); }),
            Renderer([] { return text("Machines in class") | align_right; })
        });

        table.emplace_back(Components{
            Renderer([] { return text("Count") | center; }),
            Renderer([] { return separatorDouble(); }),
            Renderer([this]
                {
                    std::string s;
                    if (not lock_)
                        s = "--";
                    else
                        s = std::to_string(std::size(console_.halted_machines_));
                    return text(s);
                }),
            Renderer([] { return separator(); }),
            Renderer([this]
                {
                    std::string s;
                    if (not lock_ or not *console_.focus() or
                            (**console_.focus()).state() != machine_t::machine_state_t::halted)
                        s = "--";
                    else
                    {
                        auto it = console_.halted_machines_.find((**console_.focus()).output());
                        if (it == std::cend(console_.halted_machines_))
                            abort("console_t::tui_t::machines_t::machines_t(console_t &)");
                        s = std::to_string(std::size(it->second));
                    }
                return text(s); })
        });

        table.emplace_back(Components{
            Renderer([] { return text("Focus") | center; }),
            Renderer([] { return separatorDouble(); }),
            classes_buttons,
            Renderer([] { return separator(); }),
            class_buttons
        });

        component_ = Container::Vertical({
            Renderer([] { return text("Machines") | center; }),
            Renderer([] { return separatorDouble(); }),
            Container::Horizontal({
                Container::Vertical(table[0]) | xflex,
                Renderer([] { return separator(); }),
                Container::Vertical(table[1]) | xflex,
                Renderer([] { return separator(); }),
                Container::Vertical(table[2]) | xflex
            }) | yflex_grow,
            Renderer([] { return separatorDouble(); }),
            Container::Horizontal({
                Container::Vertical(table[3]) | xflex,
                Renderer([] { return separator(); }),
                Container::Vertical(table[4]) | xflex,
                Renderer([] { return separator(); }),
                Container::Vertical(table[5]) | xflex
            }) | yflex_grow,
        });
        
        component_ |= size(WIDTH, EQUAL, console_.tui().width(3));
        component_ |= size(HEIGHT, EQUAL, console_.tui().height(3));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::machines_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::machines_t::component() { return component_; }

    console_t::tui_t::program_t::program_t(console_t &console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_x_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = &scroll_y_,
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Down,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | bgcolor(ftxui::Color::GrayLight)}
    {
        using namespace ftxui;

        component_ = Renderer(child_component_, [this]
            {
                if (update_)
                {
                    update_ = false;

                    std::unique_lock<std::mutex> &l = console_.tui().lock_;

                    if (not l)
                        l.lock();

                    Component old_child = child_component_;
                    std::shared_ptr<machine_t> focus = *console_.focus();

                    if (not focus)
                        child_component_ = Container::Vertical({
                                    Renderer([]{ return text("Program") | center; }),
                                    Renderer([]{ return separatorDouble();}),
                                    Renderer([]{ return text("Uninitialised console") | center; } )
                                });

                    else
                    {
                        const std::vector<std::unique_ptr<device_t>> &devices = focus->devices();
                        index_t n = std::size(devices);

                        if (std::size(dimensions_) != n + 1)
                        {
                            dimensions_.resize(n + 1);
                            dimensions_.front() = 4;
                            std::fill(std::begin(dimensions_) + 1, std::end(dimensions_),
                                (console_.tui().width(0) - 4) / n - 1);
                        }

                        Components header{Renderer([]{ return text("No."); })};

                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = devices[i]->print_name();
                            dimensions_[i + 1] = std::max(dimensions_[i + 1],
                                    static_cast<int>(std::size(s)));
                            header.emplace_back(Renderer([s = std::move(s)] { return text(s); }));
                        }

                        {
                            std::string s = devices.back()->print_name();
                            padding_ = std::size(s);
                            header.emplace_back(Renderer([s = std::move(s), &p = padding_] mutable
                                {
                                    s.resize(p, ' ');
                                    return text(s);
                                }));
                        }

                        Component header_row = resizable_row(header,
                            std::span<int>{std::begin(dimensions_), std::end(dimensions_)});

                        header_row = Renderer(header_row,
                            [header_row, &sx = scroll_x_, &sy = scroll_y_]
                                { return header_row->Render() | focusPositionRelative(sx, sy) |
                                    frame | flex; });

                        std::vector<Components> rows;

                        rows.emplace_back(Components{Renderer([]{ return text("E"); })});
                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = focus->print_encoder(i);
                            dimensions_[i + 1] = std::max(dimensions_[i + 1],
                                    static_cast<int>(std::size(s)));

                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), &d = dimensions_[i + 1]] mutable
                                {
                                    s.resize(d, ' ');
                                    return text(s);
                                }));
                        }

                        {
                            std::string s = focus->print_encoder(n - 1);
                            padding_ = std::max(padding_, std::size(s));
                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), &p = padding_] mutable
                                {
                                    s.resize(p, ' ');
                                    return text(s);
                                }));
                        }

                        rows.emplace_back(Components{Renderer([]{ return text("I"); })});
                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = focus->print_initialiser(i);
                            dimensions_[i + 1] = std::max(dimensions_[i + 1],
                                    static_cast<int>(std::size(s)));

                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), &d = dimensions_[i + 1]] mutable
                                {
                                    s.resize(d, ' ');
                                    return text(s);
                                }));
                        }

                        {
                            std::string s = focus->print_initialiser(n - 1);
                            padding_ = std::max(padding_, std::size(s));
                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), &p = padding_] mutable
                                {
                                    s.resize(p, ' ');
                                    return text(s);
                                }));
                        }

                        rows.emplace_back(Components{Renderer([]{ return text("T"); })});
                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = focus->print_terminator(i);
                            dimensions_[i + 1] = std::max(dimensions_[i + 1],
                                    static_cast<int>(std::size(s)));

                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), &d = dimensions_[i + 1]] mutable
                                {
                                    s.resize(d, ' ');
                                    return text(s);
                                }));
                        }

                        {
                            std::string s = focus->print_terminator(n - 1);
                            padding_ = std::max(padding_, std::size(s));
                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), &p = padding_] mutable
                                {
                                    s.resize(p, ' ');
                                    return text(s);
                                }));
                        }

                        const auto &instruction_set = focus->instruction_set();
                        for (index_t i = 0; i != std::size(instruction_set) / n; ++i)
                        {
                            Components row{Renderer([s = std::to_string(i)]{ return text(s); })};

                            std::vector<std::string> instruction = focus->print_instruction(i);

                            for (index_t j = 0; j != n; ++j)
                            {
                                std::string s = std::move(instruction[j]);
                                if (j != n - 1)
                                {
                                    dimensions_[j + 1] = std::max(dimensions_[j + 1],
                                        static_cast<int>(std::size(s)));

                                    row.emplace_back(Renderer(
                                        [s = std::move(s), &d = dimensions_[j + 1]] mutable
                                        {
                                            s.resize(d, ' ');
                                            return text(s);
                                        }));
                                }

                                else
                                {
                                    padding_ = std::max(padding_, std::size(s));
                                    row.emplace_back(Renderer(
                                        [s = std::move(s), &p = padding_] mutable
                                        {
                                            s.resize(p, ' ');
                                            return text(s);
                                        }));
                                }
                            }

                            rows.emplace_back(row);
                        }

                        child_component_ =
                            Container::Vertical({
                                Renderer([]{ return text("Program") | center; }),
                                Renderer([]{ return separatorDouble();}),
                                scroll_table(header, rows, slider_x_, slider_y_,
                                    std::span<int>{std::begin(dimensions_), std::end(dimensions_)},
                                    scroll_x_, scroll_y_) | yflex,
                            });
                    }

                    replace_child(component_, old_child, child_component_);
                }

                return child_component_->Render();
            });

        component_ |= size(WIDTH, EQUAL, console_.tui().width(0));
        component_ |= size(HEIGHT, EQUAL, console_.tui().height(1));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::program_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::program_t::component() { return component_; }

    console_t::tui_t::tui_t(console_t &console) : console_{console} {}

    console_t::tui_t::devices_t &console_t::tui_t::devices() { return devices_; }

    console_t::tui_t::summary_t &console_t::tui_t::summary() { return summary_; }

    console_t::tui_t::applicable_instructions_t &console_t::tui_t::applicable_instructions()
        { return applicable_instructions_; }

    console_t::tui_t::menu_t &console_t::tui_t::menu() { return menu_; }

    console_t::tui_t::machines_t &console_t::tui_t::machines() { return machines_; }
    
    console_t::tui_t::program_t &console_t::tui_t::program() { return program_; }

    ftxui::Component console_t::tui_t::component()
    {
        using namespace ftxui;

        Component devices_component = devices().component();
        Component summary_component = summary().component();
        Component applicable_instructions_component = applicable_instructions().component();
        Component menu_component = menu().component();
        Component machines_component = machines().component();
        Component program_component = program().component();

        Component container = Container::Horizontal({
            Container::Vertical({devices_component, program_component}),
            Container::Vertical({
                applicable_instructions_component,
                Container::Horizontal({
                    Container::Vertical({summary_component, machines_component}),
                    menu_component
                })
            })
        });

        return Renderer(container, [&c = console_, container, &l = lock_]
            {
                if (not l)
                    static_cast<void>(l.try_lock());
                Element ret = container->Render();

                if (l)
                    l.unlock();

                return ret;
            });
    }

    void console_t::tui_t::update_all()
    {
        devices().update();
        applicable_instructions().update();
        menu().update();
        machines().update();
        program().update();

        return;
    }


    void console_t::tui_t::loop() { screen_.Loop(component()); }

    void console_t::tui_t::exit_loop() { screen_.Exit(); }

    int console_t::tui_t::width(index_t arg) const
    {
        using namespace ftxui;

        const int a = Terminal::Size().dimx;
        const int b = a / 2;
        const int c = b / 2;

        switch (arg)
        {
            case 0:
                return b;

            case 1:
                return a - b;

            case 2:
                return c;

            case 3:
                return b - c;

            default:
                abort("console_t::tui_t::width(index_t) const");
        }
    }

    int console_t::tui_t::height(index_t arg) const
    {
        using namespace ftxui;

        const int a = Terminal::Size().dimy;
        const int b = a / 3;
        const int c = 10;

        switch (arg)
        {
            case 0:
                return b;

            case 1:
                return a - b;

            case 2:
                return c;

            case 3:
                return (a - b) - c;

            default:
                abort("console_t::tui_t::height(index_t) const");
        }
    }

    console_t::console_t() : console_t{default_n_threads()} {}

    console_t::console_t(index_t n_threads) : n_threads_{n_threads}
    {
        if (console_constructed_.exchange(true))
            throw std::runtime_error{"In Machine::console_t::console_t(index_t):\n"
                "Only a single console_t object is allowed to exist at a given point.\n"};

        return;
    }

    console_t::~console_t()
    {
        pause_ = true;

        wait();

        console_constructed_ = false;
        return;
    }

    index_t console_t::default_n_threads()
    {
        index_t r = std::thread::hardware_concurrency();
        return std::max(static_cast<index_t>(1), r);
    }

    console_t::console_state_t console_t::state() { return state_; }

    std::string console_t::load_program(std::string arg)
    {
        *program_name() = std::move(arg);
        return load_program();
    }

    console_t &console_t::initialise(std::string arg) { return initialise_all(std::move(arg)); }

    console_t &console_t::initialise_all(std::string arg)
    {
        *initialiser_string() = std::move(arg);
        initialise_all();
        return *this;
    }

    console_t &console_t::initialise_individually(const std::vector<std::string> &arg)
    {
        if (std::size(arg) + 2 != std::size(strings_))
            throw std::runtime_error{"In Machine::console_t::initialise_individually(const"
                " std::vector<std::string> &):\nInvalid argument.\n"};

        for (index_t i = 0; i != std::size(arg); ++i)
            *initialiser_string_vector(i) = arg[i];

        initialise_individually();

        return *this;
    }

    console_t &console_t::step()
    {
        wait();

        future_ = std::async(std::launch::async, [this]
            {
                {
                    std::lock_guard lock{mutex_};

                    list_t new_halted;

                    if (*focus_ and (**focus_).state() == machine_t::machine_state_t::running)
                        step(running_machines_, focus_, new_halted, blocked_machines_);

                    for (auto i = std::cbegin(new_halted); i != std::cend(new_halted);)
                    {
                        auto j = i;
                        ++i;
                        list_t &l = halted_machines_[(*j)->output()];
                        l.splice(std::cend(l), new_halted, j);
                    }
                }
            
                tui().applicable_instructions().update();
            });

        return *this;
    }

    console_t &console_t::run()
    {
        wait();

        future_ = std::async(std::launch::async, [this]
            {
                {
                    std::lock_guard lock{mutex_};

                    pause_ = false;

                    list_t new_halted;

                    while (not pause_ and *focus_ and
                           (**focus_).state() == machine_t::machine_state_t::running)
                        step(running_machines_, focus_, new_halted, blocked_machines_);

                    for (auto i = std::cbegin(new_halted); i != std::cend(new_halted);)
                    {
                        auto j = i;
                        ++i;
                        list_t &l = halted_machines_[(*j)->output()];
                        l.splice(std::cend(l), new_halted, j);
                    }
                }

                tui().applicable_instructions().update();
            });

        return *this;
    }

    console_t &console_t::step_all()
    {
        wait();

        future_ = std::async(std::launch::async, [this]
            {
                {
                    std::lock_guard lock{mutex_};

                    pause_ = false;

                    std::vector<std::thread> threads;
                    
                    index_t s = std::size(running_machines_);
                    index_t a = s / n_threads_;
                    index_t b = s % n_threads_;

                    std::vector<list_t> lists(3 * n_threads_);

                    for (index_t i = 0; i != n_threads_; ++i)
                    {
                        if (not a and not b)
                            break;

                        it_t c = std::begin(running_machines_);
                        it_t d = std::next(c, a);
                        if (b != 0)
                        {
                            --b;
                            ++d;
                        }

                        lists[3 * i].splice(std::cend(lists[3 * i]), running_machines_, c, d);
                        threads.emplace_back(
                            [&running = lists[3 * i], &halted = lists[3 * i + 1],
                             &blocked = lists[3 * i + 2], &p = pause_, this]
                            {
                                auto last = std::end(running);
                                --last;

                                for (auto it = std::begin(running); it != last and not p;)
                                {
                                    auto next = std::next(it);
                                    step(running, it, halted, blocked);
                                    it = next;
                                }

                                if (not p)
                                    step(running, last, halted, blocked);

                                return;
                            });
                    }

                    for (index_t i = 0; i != std::size(threads); ++i)
                    {
                        threads[i].join();

                        running_machines_.splice(std::cend(running_machines_), lists[3 * i]);
                        blocked_machines_.splice(std::cend(blocked_machines_), lists[3 * i + 2]);

                        list_t &a = lists[3 * i + 1];

                        for (auto j = std::cbegin(a); j != std::cend(a);)
                        {
                            auto k = j;
                            ++j;
                            list_t &l = halted_machines_[(*k)->output()];
                            l.splice(std::cend(l), a, k);
                        }
                    }
                }

                tui().applicable_instructions().update();
            });

        return *this;
    }

    console_t &console_t::run_all()
    {
        wait();

        future_ = std::async(std::launch::async, [this]
            {
                {
                    std::lock_guard lock{mutex_};

                    pause_ = false;

                    while (not pause_ and not std::empty(running_machines_) and
                           not (pause_halt_ and not std::empty(halted_machines_)))
                    {
                        std::vector<std::thread> threads;
                        
                        index_t s = std::size(running_machines_);
                        index_t a = s / n_threads_;
                        index_t b = s % n_threads_;

                        std::vector<list_t> lists(3 * n_threads_);

                        for (index_t i = 0; i != n_threads_; ++i)
                        {
                            if (not a and not b)
                                break;

                            it_t c = std::begin(running_machines_);
                            it_t d = std::next(c, a);
                            if (b != 0)
                            {
                                --b;
                                ++d;
                            }

                            lists[3 * i].splice(std::cend(lists[3 * i]), running_machines_, c, d);
                            threads.emplace_back(
                                [&running = lists[3 * i], &halted = lists[3 * i + 1],
                                 &blocked = lists[3 * i + 2], &p = pause_, this]
                                {
                                    auto last = std::end(running);
                                    --last;

                                    for (auto it = std::begin(running); it != last and not p;)
                                    {
                                        auto next = std::next(it);
                                        step(running, it, halted, blocked);
                                        it = next;
                                    }

                                    if (not p)
                                        step(running, last, halted, blocked);

                                    return;
                                });
                        }

                        for (index_t i = 0; i != std::size(threads); ++i)
                        {
                            threads[i].join();

                            running_machines_.splice(std::cend(running_machines_), lists[3 * i]);
                            blocked_machines_.splice(std::cend(blocked_machines_),
                                lists[3 * i + 2]);

                            list_t &a = lists[3 * i + 1];

                            for (auto j = std::cbegin(a); j != std::cend(a);)
                            {
                                auto k = j;
                                ++j;
                                list_t &l = halted_machines_[(*k)->output()];
                                l.splice(std::cend(l), a, k);
                            }
                        }
                    }
                }

                tui().applicable_instructions().update();

                return;
            });

        return *this;
    }

    console_t &console_t::pause()
    {
        pause_ = true;
        return *this;
    }

    console_t &console_t::launch_tui()
    {
        tui_.loop();
        return *this;
    } 

    console_t &console_t::close_tui()
    {
        tui().exit_loop();
        return *this;
    }

    bool console_t::find_output(const std::string &arg) const
    {
        for (const auto &i : halted_machines_)
            if (std::ranges::find(i.first, arg) != std::cend(i.first))
                return true;

        return false;
    }

    bool console_t::find_output(const std::vector<std::string> &arg) const
    {
        if (std::size(halted_machines_) != 1)
            return false;
        
        return std::cbegin(halted_machines_)->first == arg;
    }

    void console_t::wait()
    {
        if (future_.valid())
            future_.wait();
        return;
    }

    std::vector<std::vector<std::string>> console_t::output() const
    {
        std::vector<std::vector<std::string>> ret;
        ret.reserve(std::size(halted_machines_));

        for (const auto &i : halted_machines_)
            ret.emplace_back(i.first);

        return ret;
    }

    std::atomic<bool> &console_t::pause_halt() { return pause_halt_;}

    void console_t::clear() noexcept
    {
        pause_ = true;
        wait();
        invalid_machines_.clear();
        running_machines_.clear();
        halted_machines_.clear();
        blocked_machines_.clear();
        focus_ = std::begin(null_list_);
        pause_ = false;
        state_ = console_state_t::empty;
        instruction_counter_ = 0;
        strings_.resize(2);
        strings_[0]->clear();
        strings_[1]->clear();
        tui().devices().input_strings().clear();
        tui().menu().strings().resize(3);

        return;
    }

    void console_t::reset()
    {
        if (state_ == console_state_t::empty)
            abort("console_t::reset()");

        ptr_t temp{nullptr};
        if (not std::empty(invalid_machines_))
            temp = std::move(invalid_machines_.front());
        else if (not std::empty(running_machines_))
            temp = std::move(running_machines_.front());
        else if (not std::empty(blocked_machines_))
            temp = std::move(blocked_machines_.front());
        else
        {
            for (auto &i : halted_machines_)
                if (not std::empty(i.second))
                {
                    temp = std::move(i.second.front());
                    break;
                }

            if (not temp)
                abort("console_t::reset()");
        }

        invalid_machines_.clear();
        running_machines_.clear();
        halted_machines_.clear();
        blocked_machines_.clear();

        temp->initialise("");
        switch(temp->state())
        {
            case (machine_t::machine_state_t::invalid):
            {
                invalid_machines_.emplace_back(std::move(temp));
                focus_ = std::begin(invalid_machines_);
                break;
            }
            case (machine_t::machine_state_t::running):
            {
                running_machines_.emplace_back(std::move(temp));
                focus_ = std::begin(running_machines_);
                break;
            }
            case (machine_t::machine_state_t::halted):
            {
                const std::vector<std::string> &o = temp->output();
                list_t &l = halted_machines_[o];
                focus_ = l.insert(std::cend(l), std::move(temp));
                break;
            }
            case (machine_t::machine_state_t::blocked):
            {
                blocked_machines_.emplace_back(std::move(temp));
                focus_ = std::begin(blocked_machines_);
                break;
            }
        }

        pause_ = false;
        state_ = static_cast<console_state_t>(console_state_t::program_loaded |
            (*focus_)->deterministic());

        instruction_counter_ = 0;

        return;
    }

    void console_t::step(list_t &list, it_t it, list_t &halted, list_t &blocked)
    {
        if (not *it)
            abort("console_t::step(list_t &, it_t, list_t &, list_t &)");

        machine_t &m = **it;
        machine_t::machine_state_t s = m.state();
        
        index_t n = m.n_applicable_instructions();

        if (n != 0)
        {
            index_t sel = m.selected_instruction();
            if (sel == negative_1)
                sel = 0;

            for (index_t i = 0; i != n; ++i)
            {
                if (i == sel)
                    continue;

                ptr_t a = std::make_shared<machine_t>(m);

                a->select_instruction(i);
                a->next();
                ++instruction_counter();

                if (a->state() == machine_t::machine_state_t::halted)
                    halted.emplace_back(std::move(a));
                else if (a->state() == machine_t::machine_state_t::blocked)
                    blocked.emplace_back(std::move(a));
                else
                    list.emplace_back(std::move(a));
            }
            
            m.select_instruction(sel);
            m.next();
            ++instruction_counter();
        }

        if (m.state() == machine_t::machine_state_t::halted)
            halted.splice(std::cend(halted), list, it);

        else if (m.state() == machine_t::machine_state_t::blocked)
            blocked.splice(std::cend(blocked), list, it);

        return;
    }

    std::string console_t::load_program()
    {
        std::string ret;
        std::lock_guard lock{mutex_};

        {
            bool error_in_loading = true;

            try
            {
                std::ifstream stream{*program_name()};
                ptr_t new_machine = std::make_shared<machine_t>(stream);

                if (not new_machine or new_machine->state() != machine_t::machine_state_t::invalid)
                    abort("console_t::load_program()");

                error_in_loading = false;

                clear();

                invalid_machines_ = {std::move(new_machine)};
                focus_ = std::begin(invalid_machines_);

                state_ = static_cast<console_state_t>(console_state_t::program_loaded |
                    (*focus_)->deterministic());
            }

            catch (std::runtime_error &e)
            {
                ret = e.what();
                *tui().menu().strings()[0] = "Error loading the program.";

                if (not error_in_loading)
                {
                    clear();
                    tui().update_all();
                }

                return ret;
            }

            if (*focus_)
                for (index_t i = 0; i != std::size((*focus_)->devices()); ++i)
                {
                    tui().devices().input_strings().emplace_back(std::make_shared<std::string>());
                    strings_.emplace_back(std::make_shared<std::string>());
                    tui().menu().strings().emplace_back(strings_.back());
                }

            *tui().menu().strings()[0] = "Program loaded.";
            tui().menu().tab_selected() = 0;
        }

        tui().update_all();

        return ret;
    }

    void console_t::initialise_all()
    {
        {
            std::lock_guard lock{mutex_};

            reset();

            const std::string &s = *initialiser_string();

            (*focus_)->initialise(s);

            if ((**focus_).state() == machine_t::machine_state_t::running)
            {
                running_machines_.splice(std::cend(running_machines_), invalid_machines_);
                running_machines_.splice(std::cend(running_machines_), blocked_machines_);

                for (auto &i : halted_machines_)
                    running_machines_.splice(std::cend(running_machines_), i.second);

                if (std::size(running_machines_) != 1)
                    abort("console_t::initialise_all()");

                focus_ = std::begin(running_machines_);
            }
            else if ((**focus_).state() == machine_t::machine_state_t::blocked)
            {
                blocked_machines_.splice(std::cend(blocked_machines_), invalid_machines_);
                blocked_machines_.splice(std::cend(blocked_machines_), running_machines_);

                for (auto &i : halted_machines_)
                    blocked_machines_.splice(std::cend(running_machines_), i.second);

                if (std::size(blocked_machines_) != 1)
                    abort("console_t::initialise_all()");

                focus_ = std::begin(blocked_machines_);
            }
            else
                abort("console_t::initialise_all()");

            for (index_t i = 0; i != std::size((*focus_)->devices()); ++i)
                *(tui().devices().input_strings()[i]) = s;

            state_ = static_cast<console_state_t>((*focus_)->deterministic() |
                    console_state_t::program_loaded | console_state_t::running);
        }

        tui().applicable_instructions().update();

        return;
    }

    void console_t::initialise_individually()
    {
        {
            std::lock_guard lock{mutex_};

            reset();

            std::vector<std::string> input;
            for (index_t i = 0; i != std::size((*focus_)->devices()); ++i)
            {
                const std::string &s = *initialiser_string_vector(i);
                input.emplace_back(s);
                *(tui().devices().input_strings()[i]) = s;
            }

            (*focus_)->initialise(
                    std::span<const std::string>{std::cbegin(input), std::cend(input)});

            if ((**focus_).state() == machine_t::machine_state_t::running)
            {
                running_machines_.splice(std::cend(running_machines_), invalid_machines_);
                running_machines_.splice(std::cend(running_machines_), blocked_machines_);

                for (auto &i : halted_machines_)
                    running_machines_.splice(std::cend(running_machines_), i.second);

                if (std::size(running_machines_) != 1)

                    abort("console_t::initialise_individually()");

                focus_ = std::begin(running_machines_);
            }
            else if ((**focus_).state() == machine_t::machine_state_t::blocked)
            {
                blocked_machines_.splice(std::cend(blocked_machines_), invalid_machines_);
                blocked_machines_.splice(std::cend(blocked_machines_), running_machines_);

                for (auto &i : halted_machines_)
                    blocked_machines_.splice(std::cend(running_machines_), i.second);

                if (std::size(blocked_machines_) != 1)
                    abort("console_t::initialise_individually()");

                focus_ = std::begin(blocked_machines_);
            }
            else
                abort("console_t::initialise_individually()");

            state_ = static_cast<console_state_t>((*focus_)->deterministic() |
                    console_state_t::program_loaded | console_state_t::running);
        }

        tui().applicable_instructions().update();

        return;
    }

    void console_t::change_focus(machine_t::machine_state_t state, index_t c)
    {
        using enum machine_t::machine_state_t;

        auto empty = [&s = *tui().menu().strings()[0]]
            { s = "Empty list"; };
        auto front = [&s = *tui().menu().strings()[0]]
            { s = "Focus already at the front of the list"; };
        auto back = [&s = *tui().menu().strings()[0]]
            { s = "Focus already at the end of the list"; };
        auto out = [&s = *tui().menu().strings()[0]]
            { s = "Argument out of range"; };

        if (state == halted)
        {
            if (std::empty(halted_machines_))
                empty();
            else if (c != negative_1 and c != negative_2)
            {
                if (c >= std::size(halted_machines_))
                    out;
                else
                    focus_ = std::begin(std::next(std::begin(halted_machines_), c)->second);
            }
            else
            {
                if (not *focus_ or (**focus_).state() != state)
                    focus_ = std::begin(std::begin(halted_machines_)->second);
                else if (c == negative_2)
                {
                    auto it = halted_machines_.find((**focus_).output());
                    if (it == std::end(halted_machines_))
                        abort("console_t::change_focus(machine_t::machine_state_t, index_t)");
                    if (it == std::begin(halted_machines_))
                        front();
                    else
                        focus_ = std::begin((--it)->second);
                }
                else
                {
                    auto it = halted_machines_.find((**focus_).output());
                    if (it == std::end(halted_machines_))
                        abort("console_t::change_focus(machine_t::machine_state_t, index_t)");
                    if (it == --std::end(halted_machines_))
                        back();
                    else
                        focus_ = std::begin((++it)->second);
                }
            }
        }
        else
        {
            list_t &l = [state, this] mutable -> list_t &
            {
                switch (state)
                {
                    case (invalid) :
                        return invalid_machines_;
                    case (running) :
                        return running_machines_;
                    case (blocked) :
                        return blocked_machines_;
                    default :
                        abort("console_t::change_focus(machine_t::machine_state_t, index_t)");
                }
            }();

            if (std::empty(l))
                empty();
            else if (c != negative_1 and c != negative_2)
            {
                if (c >= std::size(l))
                    out();
                else
                    focus_ = std::next(std::begin(l), c);
            }
            else
            {
                if (not *focus_ or (**focus_).state() != state)
                    focus_ = std::begin(l);
                else if (c == negative_2)
                {
                    if (focus_ == std::begin(l))
                        front();
                    else
                        --focus_;
                }
                else
                {
                    if (focus_ == --std::end(l))
                        back();
                    else
                        ++focus_;
                }
            }
        }

        tui().applicable_instructions().update();

        return;
    }

    void console_t::change_focus_class(index_t c)
    {
        using enum machine_t::machine_state_t;

        auto empty = [&s = *tui().menu().strings()[0]]
            { s = "The current focus has not halted"; };
        auto front = [&s = *tui().menu().strings()[0]]
            { s = "Focus already at the front of the list"; };
        auto back = [&s = *tui().menu().strings()[0]]
            { s = "Focus already at the end of the list"; };
        auto out = [&s = *tui().menu().strings()[0]]
            { s = "Argument out of range"; };

        if (not *focus_ or (**focus_).state() != halted)
        {
            empty();
            return;
        }

        auto it = halted_machines_.find((**focus_).output());
        if (it == std::end(halted_machines_))
            abort("console_t::change_focus_class(index_t)");

        list_t &l = it->second;

        if (std::empty(l))
            abort("console_t::change_focus_class(index_t)");

        if (c != negative_1 and c != negative_2)
        {
            if (c >= std::size(l))
                out();
            else
                focus_ = std::next(std::begin(l), c);
        }
        else if (c == negative_2)
        {
            if (focus_ == std::begin(l))
                front();
            else
            --focus_;
        }
        else
        {
            if (focus_ == --std::end(l))
                back();
            else
                ++focus_;
        }

        tui().applicable_instructions().update();

        return;
    }

    std::shared_ptr<std::string> console_t::program_name() { return strings_[0]; }

    std::shared_ptr<std::string> console_t::initialiser_string() { return strings_[1]; }
    
    std::shared_ptr<std::string> console_t::initialiser_string_vector(index_t arg)
        { return strings_[2 + arg]; }

    console_t::it_t console_t::focus() { return focus_; }

    console_t::tui_t &console_t::tui() { return tui_; }

    std::mutex &console_t::mutex() { return mutex_; }

    std::atomic<index_t> &console_t::instruction_counter() { return instruction_counter_; }
}

