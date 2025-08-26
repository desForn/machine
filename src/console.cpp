#include "console.hpp"
#include "tape.hpp"

#include <algorithm>
#include <cmath>
#include <cassert>

namespace Machine
{
    static ftxui::Component resizable_row(ftxui::Components &arg,
            std::vector<std::shared_ptr<int>> &sizes)
    {
        using namespace ftxui;

        if (std::empty(arg) or std::size(arg) > std::size(sizes) + 1)
            throw std::runtime_error{"In Machine::resizable_row(ftxui::Components &,"
                "std::vector<int> &):\nInvalid argument.\n"};
        
        Component ret = arg.back();

        auto i = std::rbegin(arg) + 1;
        auto j = std::begin(sizes) + std::size(arg) - 2;
        for(; i != std::rend(arg); ++i, --j)
            ret = ResizableSplitLeft(*i, ret, j->get());

        ret = Renderer(ret, [s = sizes, ret] { return ret->Render(); });

        return ret;
    }

    static ftxui::Component resizable_table(std::vector<ftxui::Components> &arg,
            std::vector<std::shared_ptr<int>> &horizontal_sizes,
            std::vector<std::shared_ptr<int>> &vertical_sizes)
    {
        using namespace ftxui;

        std::runtime_error error{"In Machine::resizable_table(std::vector<std::vector"
            "<Component>> &, std::vector<int> &, std::vector<int> &):\nInvalid argument.\n"};

        if (std::size(arg) > std::size(vertical_sizes) + 1)
            throw error;

        Component ret = resizable_row(arg.back(), horizontal_sizes);
        
        auto i = std::rbegin(arg) + 1;
        auto j = std::begin(vertical_sizes) + std::size(arg) - 2;
        for(; i != std::rend(arg); ++i, --j)
            ret = ResizableSplitTop(resizable_row(*i, horizontal_sizes), ret, j->get());
        
        ret = Renderer(ret, [v = vertical_sizes, ret] { return ret->Render(); });

        return ret;
    }

    static ftxui::Component resizable_table(std::vector<ftxui::Components> &arg,
        std::vector<std::shared_ptr<int>> &horizontal_sizes, bool header = false)
    {
        using namespace ftxui;

        std::runtime_error error{"In Machine::resizable_table(std::vector<std::vector"
            "<Component>> &, std::vector<int> &):\nInvalid argument.\n"};

        if (std::empty(arg))
            throw error;

        Components rows;
        
        bool first_row = true;
        for (auto &i : arg)
        {
            rows.emplace_back(resizable_row(i, horizontal_sizes));
            if (first_row and header)
                rows.emplace_back(Renderer([]{ return separatorDouble(); }));
            else
                rows.emplace_back(Renderer([]{ return separator(); }));
            first_row = false;
        }
        
        return Container::Vertical(rows);
    }

    static ftxui::Component scroll_table(ftxui::Components &header,
        std::vector<ftxui::Components> &rows,
        ftxui::Component &slider_x, ftxui::Component &slider_y,
        std::vector<std::shared_ptr<int>> &horizontal_sizes,
        std::shared_ptr<float> scroll_x, std::shared_ptr<float> scroll_y)
    {
        using namespace ftxui;

        Component header_row = resizable_row(header, horizontal_sizes);
        header_row = Renderer(header_row, [header_row, scroll_x]
            { return header_row->Render() | focusPositionRelative(*scroll_x, 0)
                | frame | flex; }) | size(HEIGHT, EQUAL, 1);

        Component table = resizable_table(rows, horizontal_sizes);

        table = Renderer(table, [table, sx = scroll_x, sy = scroll_y]
            { return table->Render() | focusPositionRelative(*sx, *sy) | frame | flex; });

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

    static ftxui::Component scroll_table(
        std::vector<ftxui::Components> &rows,
        ftxui::Component &slider_x, ftxui::Component &slider_y,
        std::vector<std::shared_ptr<int>> &horizontal_sizes,
        std::shared_ptr<float> scroll_x, std::shared_ptr<float> scroll_y, bool align = true)
    {
        using namespace ftxui;

        Component table = resizable_table(rows, horizontal_sizes);

        if (align)
            table = Renderer(table, [table, sx = scroll_x, sy = scroll_y, n = std::size(rows)]
            {
                float position_y = std::round(*sy * n) + 0.5f / n;
                return table->Render() | focusPositionRelative(*sx, position_y) | frame | flex;
            });
        else
            table = Renderer(table, [table, sx = scroll_x, sy = scroll_y]
                { return table->Render() | focusPositionRelative(*sx, *sy) | frame | flex; });

        Component ret =
        Container::Vertical({
            Container::Horizontal({
                table | yflex_grow | xflex,
                slider_y
            }) | yflex,
            slider_x
        });
                
        return ret;
    }

    static ftxui::Component scroll_horizontal(ftxui::Component &component,
        ftxui::Component &slider, std::shared_ptr<float> scroll)
    {
        using namespace ftxui;

        Component c = Renderer(component, [component, s = std::move(scroll)]
            { return component->Render() | focusPositionRelative(*s, 0) | frame | flex; });

        return Container::Vertical({c, slider});
    }

    static void replace_child(ftxui::Component &root, ftxui::Component &obsolete_child,
        ftxui::Component &new_child)
    {
        using namespace ftxui;

        Component parent{nullptr};

        for (Component i = root; not parent; i = i->ChildAt(0))
        {
            if (not i or i->ChildCount() != 1)
            {
                std::cerr << "In Machine::replace_child(ftxui::Component &, ftxui::Component &,"
                    "ftxui::Component &):\nFatal error.\nAborting...\n" << std::flush;
                std::abort();
            }
            
            if (i->ChildAt(0).get() == obsolete_child.get())
                parent = i;
        }

        parent->DetachAllChildren();
        parent->Add(new_child);
        new_child = parent->ChildAt(0);

        return;
    }

    console_t::tui_t::devices_t::devices_t(console_t *console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_x_.get(),
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_y_.get(),
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

                    Component old_child = child_component_;

                    std::shared_ptr<machine_t> focus = console_->focus();

                    for (auto &i : dimensions_)
                        *i = 6;

                    Component top_left_cell = Renderer([d = dimensions_[0],
                            s = std::string{"Device"}] mutable
                        {
                            s.resize(*d, ' ');
                            return text(s);
                        });

                    Component top_row;

                    *padding_ = 16;

                    top_row = Renderer([p = padding_, s = std::string{"Next instruction"}] mutable
                        {
                            s.resize(*p, ' ');
                            return text(s);
                        });

                    top_row = ResizableSplitLeft(
                        Renderer([] { return text("State"); }),
                        top_row, dimensions_[3].get());

                    top_row = ResizableSplitLeft(
                        Renderer([] { return text("Output"); }),
                        top_row, dimensions_[2].get());

                    top_row = ResizableSplitLeft(
                        Renderer([] { return text("Input"); }),
                        top_row, dimensions_[1].get());

                    Component scroll_top_row = Renderer(top_row, [top_row, sx = scroll_x_]
                        { return top_row->Render() | focusPositionRelative(*sx, 0) | frame | flex; }
                    );

                    if (not focus)
                        child_component_ = Container::Vertical({
                            Container::Horizontal({top_left_cell, scroll_top_row}),
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
                            *dimensions_[0] =
                                std::max(*dimensions_[0], static_cast<int>(std::size(device_name)));

                            if (typeid(*devices[i]) != typeid(tape_t))
                                left_column.emplace_back(
                                    Renderer(
                                        [s = std::move(device_name), &d = *dimensions_[0]] mutable
                                        {
                                            s.resize(d, ' ');
                                            return text(s);
                                        }));

                            else
                                left_column.emplace_back(Container::Vertical({
                                    Renderer([s = std::move(device_name)] { return text(s); }),
                                    Renderer([] { return text(""); })}));
                            left_column.emplace_back(Renderer([]{ return separator(); }));

                            rows.emplace_back(Components{Renderer(
                                [s = input_strings_[i], d = dimensions_[1]]
                                {
                                    *d = std::max(*d, static_cast<int>(std::size(*s)));
                                    return text(*s) | xflex_grow;
                                })});

                            rows.back().emplace_back(Renderer([f = focus, i, d = dimensions_[2]]
                                {
                                    std::string s = f->output()[i];
                                    *d = std::max(*d, static_cast<int>(std::size(s)));
                                    return text(std::move(s));
                                }));

                            if (typeid(*devices[i]) != typeid(tape_t))
                                rows.back().emplace_back(Renderer([f = focus, i, d = dimensions_[3]]
                                    {
                                        machine_t::machine_state_t state = f->state();
                                        
                                        std::string s;

                                        if (state == machine_t::machine_state_t::invalid)
                                            s = "Invalid";
                                        else if (state == machine_t::machine_state_t::halted)
                                            s = "Halted";
                                        else
                                            s = f->devices()[i]->print_state();

                                        *d = std::max(*d, static_cast<int>(std::size(s)));
                                        return text(std::move(s));
                                    }));

                            else
                                rows.back().emplace_back(Container::Vertical({
                                    Renderer([f = focus, i, d = dimensions_[1]]
                                    {
                                        machine_t::machine_state_t state = f->state();
                                        
                                        std::string s;
                                        if (state == machine_t::machine_state_t::invalid)
                                            s = "Invalid";
                                        else if (state == machine_t::machine_state_t::halted)
                                            s = "Halted";
                                        else
                                            s = f->devices()[i]->print_state();
                                        
                                        *d = std::max(*d, static_cast<int>(std::size(s)));
                                        return text(std::move(s));
                                    }),
                                    Renderer([f = focus, i]
                                    {
                                        std::string s;

                                        machine_t::machine_state_t state = f->state();

                                        if (state != machine_t::machine_state_t::invalid and
                                            state != machine_t::machine_state_t::halted)
                                        {
                                            const tape_t &tape = dynamic_cast<const tape_t &>(
                                                    *f->devices()[i]);
                                            s.resize(tape.string().pos(), ' ');
                                            s.push_back('^');
                                        }

                                        return text(s);
                                    })}));

                            rows.back().emplace_back(
                                Renderer([f = focus, i, p = padding_]
                                    {
                                        std::string s = f->next_instruction(i);
                                        *p = std::max(*p, std::size(s));
                                        s.resize(*p, ' ');
                                        return text(std::move(s));
                                    })
                            );
                        }

                        std::vector<std::shared_ptr<int>> dimensions{
                            std::begin(dimensions_) + 1, std::end(dimensions_)};
                        Component table = resizable_table(rows, dimensions);
                        table = Renderer(table, [table, sx = scroll_x_, sy = scroll_y_]
                            {
                                return table->Render() | focusPositionRelative(*sx, *sy) | frame
                                | flex;
                            });

                        Component scroll_left_column = Container::Vertical(left_column);

                        scroll_left_column = Renderer(scroll_left_column,
                            [scroll_left_column, sy = scroll_y_, n = std::size(left_column)]
                            {
                                return scroll_left_column->Render() | focusPositionRelative(0, *sy)
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

        component_ |= size(WIDTH, EQUAL, console_->tui().width(0));
        component_ |= size(HEIGHT, EQUAL, console_->tui().height(0));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::devices_t::update() { update_ = true; }

    std::vector<std::shared_ptr<std::string>> &console_t::tui_t::devices_t::input_strings()
        { return input_strings_; }

    ftxui::Component &console_t::tui_t::devices_t::component() { return component_; }

    console_t::tui_t::summary_t::summary_t(console_t *console) : console_{console}
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
                Renderer([&c = console_->state_]
                    {
                        index_t a = static_cast<index_t>(c);
                        std::string s;
                        if (a & console_t::console_state_t::stopped)
                            s = "Stopped";
                        else if (a & console_t::console_state_t::running)
                            s = "Running";
                        else if (a & console_t::console_state_t::initialised)
                            s = "Initialised";
                        else if (a & console_t::console_state_t::program_loaded)
                            s = "Program loaded";
                        else 
                            s = "Empty";

                        return text(s);
                    }),
                Renderer([] { return separator(); }),
                Renderer([&c = console_->state_]
                    {
                        std::string s;
                        if (c == console_t::console_state_t::empty)
                            s = "Not applicable";
                        else if (c & console_t::console_state_t::deterministic)
                            s = "Yes";
                        else
                            s = "No";

                        return text(s);
                    }),
                Renderer([] { return separator(); }),
                Renderer([&c = console_->instruction_counter_]
                    {
                        return text(std::to_string(c));
                    }),
                Renderer([] { return separator(); }),
                Renderer([c = console_]
                    {
                        std::shared_ptr<machine_t> f = c->focus();

                        std::string s;
                        if (f)
                            s = std::to_string(f->computation_size());
                        else
                            s = "Not applicable";

                        return text(s);
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

        component_ |= size(WIDTH, EQUAL, console_->tui().width(3));
        component_ |= size(HEIGHT, EQUAL, console_->tui().height(2));
        component_ |= borderHeavy;

        return;
    }

    ftxui::Component &console_t::tui_t::summary_t::component() { return component_; }

    console_t::tui_t::applicable_instructions_t::applicable_instructions_t(console_t *console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_x_.get(),
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_y_.get(),
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Down,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)}
    {
        using namespace ftxui;

        component_ = Renderer(child_component_, [focus = console_->focus_, this] mutable
        {
            if (update_)
            {
                update_ = false;

                Component old_child = child_component_;

                focus = console_->focus_;

                bool_vector_.clear();

                if (not focus)
                    child_component_ = Container::Vertical({
                        Renderer([]{ return text("Available Instructions") | center; }),
                        Renderer([]{ return separatorDouble(); }),
                        Renderer([]{ return text("Uninitialised console") | center; } )
                    });

                else
                {
                    const std::vector<std::unique_ptr<device_t>> &devices = focus->devices();
                    index_t n = std::size(devices);

                    std::vector<std::string> applicable_instructions =
                        focus->print_applicable_instructions();

                    index_t s = std::size(applicable_instructions) / n;

                    bool_vector_.resize(s);

                    *padding_ = std::size(devices.back()->print_name());

                    for (auto i = std::cbegin(applicable_instructions);
                            i != std::cend(applicable_instructions); i += n)
                        *padding_ = std::max(*padding_, std::size(*(i + n - 1)));

                    if (std::size(dimensions_) != n + 1)
                    {
                        dimensions_.clear();
                        for (index_t i = 0; i != n + 1; ++i)
                            dimensions_.emplace_back(
                                std::make_shared<int>((console_->tui().width(3) - 4) /
                                std::size(devices)));
                        *(dimensions_.front()) = 4;
                    }

                    Components header{Renderer([]{ return text(""); })};

                    for (auto i = std::begin(devices); i != std::end(devices) - 1; ++i)
                        header.emplace_back(Renderer([s = (*i)->print_name()] { return text(s); }));

                    {
                        std::string s = devices.back()->print_name();
                        s.resize(*padding_, ' ');
                        header.emplace_back(Renderer([s = std::move(s)] { return text(s); }));
                    }

                    Component header_row = resizable_row(header, dimensions_);
                    header_row = Renderer(header_row, [header_row, sx = scroll_x_]
                        { return header_row->Render() | focusPositionRelative(*sx, 0) |
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
                            s.resize(*padding_, ' ');
                            rows.back().emplace_back(Renderer([s = std::move(s)]
                                { return text(s); }));
                        }

                        child_component_ =
                            Container::Vertical({
                                Renderer([]{ return text("Applicable Instructions") | center; }),
                                Renderer([]{ return separatorDouble(); }),
                                scroll_table(header, rows, slider_x_, slider_y_, dimensions_,
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

            if (not std::empty(bool_vector_))
                focus->select_instruction(selected_);

            return child_component_->Render();
        });

        component_ |= size(WIDTH, EQUAL, console_->tui().width(1));
        component_ |= size(HEIGHT, EQUAL, console_->tui().height(0));
        component_ |= yflex_grow;
        component_ |= borderHeavy;

        return;
    }
    
    void console_t::tui_t::applicable_instructions_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::applicable_instructions_t::component()
        { return component_; }

    console_t::tui_t::menu_t::menu_t(console_t *console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_x_.get(),
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | ftxui::bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_y_.get(),
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
        strings_.emplace_back(console_->strings_[0]);
        strings_.emplace_back(console_->strings_[1]);


        component_ = Renderer(child_component_, [this]
            {
                if (update_)
                {
                    update_ = false;

                    Component old_child = child_component_;

                    std::shared_ptr<machine_t> focus = console_->focus();

                    strings_[0]->clear();

                    bool empty = console_->state_ == console_t::console_state_t::empty;

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
                            Button("Initialise", [c = console_] { c->initialise_individually(); },
                            button_style_));
                    }


                    Component step_button =
                        Button("Step", [c = console_] { c->step(); }, button_style_);
                    Component run_button =
                        Button("Run", [c = console_] { c->run(); }, button_style_);
                    Component step_all_button =
                        Button("Step all", [c = console_] { c->step_all(); }, button_style_);
                    Component run_all_button =
                        Button("Run all", [c = console_] { c->run_all(); }, button_style_);

                    Component buttons = Container::Vertical({
                        Container::Horizontal({
                            Renderer(step_button, [f = focus, s = step_button, &t = console_->tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not f or f->state() != machine_t::machine_state_t::running)
                                        return ret | inverted;
                                    else
                                        return ret;
                                }),
                            Renderer(run_button, [f = focus, s = run_button, &t = console_->tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not f or f->state() != machine_t::machine_state_t::running)
                                        return ret | inverted;
                                    else
                                        return ret;
                                })
                            }),
                        Container::Horizontal({
                            Renderer(step_all_button,
                                [f = focus, s = step_all_button, &t = console_->tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not f or f->state() != machine_t::machine_state_t::running)
                                        return ret | inverted;
                                    else
                                        return ret;
                                }),
                            Renderer(run_all_button,
                                [f = focus, s = run_all_button, &t = console_->tui()]
                                {
                                    Element ret = s->Render();
                                    ret |= size(WIDTH, EQUAL, (t.width(2) - 3) / 2);

                                    if (not f or f->state() != machine_t::machine_state_t::running)
                                        return ret | inverted;
                                    else
                                        return ret;
                                })
                            })
                        });

                    Components rows;
                    rows.emplace_back(Renderer(buttons, [buttons, &t = console_->tui()]
                        { return hbox({buttons->Render()}) | size(WIDTH, EQUAL, t.width(2) - 3); }
                    ));

                    rows.emplace_back(Renderer([]{ return separatorDouble(); }));
                    rows.emplace_back(Toggle(&tab_entries_, tab_selected_.get()));
                    rows.emplace_back(Renderer([]{ return separatorDouble(); }));

                    Component a;
                    if (empty)
                        a = Renderer([]{ return text("Empty console") | center; });
                    else
                        a = Container::Vertical({
                                Input(strings_[2].get(), "Input string", input_style_),
                                Button("Initialise", [c = console_] { c->initialise_all(); },
                                    button_style_)
                            });

                    Component b = Container::Vertical(initialisers);

                    Component c = Container::Vertical({
                                Input(strings_[1].get(), "Enter the program name", input_style_),
                                Button("Load program", [c = console_] { c->load_program(); },
                                    button_style_)
                            });

                    rows.emplace_back(Container::Tab({
                        Renderer(a, [a, &t = console_->tui()]
                            { return hbox({a->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }),
                        Renderer(b, [b, &t = console_->tui()]
                            { return hbox({b->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }),
                        Renderer(c, [c, &t = console_->tui()]
                            { return hbox({c->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }),
                        }, tab_selected_.get()
                    ));

                    rows.emplace_back(Renderer([]{ return separatorDouble(); }));

                    Component d = Button("Quit", tui_t::screen().ExitLoopClosure(), button_style_);

                    rows.emplace_back(Renderer(d, [d, &t = console_->tui()]
                        { return hbox({d->Render() | size(WIDTH, EQUAL, t.width(2) - 3)}); }));

                    Component container = Container::Vertical(rows);
                    Component table = Renderer(container,
                        [container, sx = scroll_x_, sy = scroll_y_]
                            {
                                return container->Render() | focusPositionRelative(*sx, *sy) |
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

        component_ |= size(WIDTH, EQUAL, console_->tui().width(2));
        component_ |= size(HEIGHT, EQUAL, console_->tui().height(1));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::menu_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::menu_t::component() { return component_; }

    std::vector<std::shared_ptr<std::string>> &console_t::tui_t::menu_t::strings()
        { return strings_; }

    console_t::tui_t::machines_t::machines_t(console_t *console) : console_{console}
    {
        using namespace ftxui;

        component_ = Container::Vertical({
                Renderer([]{ return text("Machines") | center; }),
                Renderer([]{ return separatorDouble();}),
                Renderer([]{ return text("Unimplemented") | center; })});


        component_ |= size(WIDTH, EQUAL, console_->tui().width(3));
        component_ |= size(HEIGHT, EQUAL, console_->tui().height(3));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::machines_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::machines_t::component() { return component_; }

    console_t::tui_t::program_t::program_t(console_t *console) :
        console_{console},
        slider_x_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_x_.get(),
            .min = 0.f,
            .max = 1.f,
            .increment = 1e-2f,
            .direction = ftxui::Direction::Right,
            .color_active = ftxui::Color::Black,
            .color_inactive = ftxui::Color::Black
        }) | bgcolor(ftxui::Color::GrayLight)},
        slider_y_{ftxui::Slider<float>(ftxui::SliderOption<float>{
            .value = scroll_y_.get(),
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

                    Component old_child = child_component_;
                    std::shared_ptr<machine_t> focus = console_->focus();

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
                            dimensions_.clear();
                            int width = (console_->tui().width(0) - 4) / n - 1;
                            for (index_t i = 0; i != n + 1; ++i)
                                dimensions_.emplace_back(std::make_shared<int>(width));
                            *(dimensions_.front()) = 4;
                        }

                        Components header{Renderer([]{ return text("No."); })};

                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = devices[i]->print_name();
                            *(dimensions_[i + 1]) = std::max(*(dimensions_[i + 1]),
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

                        Component header_row = resizable_row(header, dimensions_);
                        header_row = Renderer(header_row,
                            [header_row, sx = scroll_x_, sy = scroll_y_]
                                { return header_row->Render() | focusPositionRelative(*sx, *sy) |
                                    frame | flex; });

                        std::vector<Components> rows;

                        rows.emplace_back(Components{Renderer([]{ return text("E"); })});
                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = focus->print_encoder(i);
                            *(dimensions_[i + 1]) = std::max(*(dimensions_[i + 1]),
                                    static_cast<int>(std::size(s)));

                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), d = dimensions_[i + 1]] mutable
                                {
                                    s.resize(*d, ' ');
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
                            *(dimensions_[i + 1]) = std::max(*(dimensions_[i + 1]),
                                    static_cast<int>(std::size(s)));

                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), d = dimensions_[i + 1]] mutable
                                {
                                    s.resize(*d, ' ');
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

                        rows.emplace_back(Components{Renderer([]{ return text("T"); })});
                        for (index_t i = 0; i != n - 1; ++i)
                        {
                            std::string s = focus->print_terminator(i);
                            *(dimensions_[i + 1]) = std::max(*(dimensions_[i + 1]),
                                    static_cast<int>(std::size(s)));

                            rows.back().emplace_back(Renderer(
                                [s = std::move(s), d = dimensions_[i + 1]] mutable
                                {
                                    s.resize(*d, ' ');
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
                                    *(dimensions_[j + 1]) = std::max(*(dimensions_[j + 1]),
                                        static_cast<int>(std::size(s)));

                                    row.emplace_back(Renderer(
                                        [s = std::move(s), d = dimensions_[j + 1]] mutable
                                        {
                                            s.resize(*d, ' ');
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
                                scroll_table(header, rows, slider_x_, slider_y_, dimensions_,
                                    scroll_x_, scroll_y_) | yflex,
                            });

                    }

                    replace_child(component_, old_child, child_component_);
                }

                return child_component_->Render();
            });

        component_ |= size(WIDTH, EQUAL, console_->tui().width(0));
        component_ |= size(HEIGHT, EQUAL, console_->tui().height(1));
        component_ |= borderHeavy;

        return;
    }

    void console_t::tui_t::program_t::update() { update_ = true; }

    ftxui::Component &console_t::tui_t::program_t::component() { return component_; }

    ftxui::ScreenInteractive console_t::tui_t::screen_{ftxui::ScreenInteractive::Fullscreen()};

    console_t::tui_t::tui_t(console_t *console) : console_{console} {}

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

        return Container::Horizontal({
            Container::Vertical({devices_component, program_component}),
            Container::Vertical({
                applicable_instructions_component,
                Container::Horizontal({
                    Container::Vertical({summary_component, machines_component}),
                    menu_component
                })
            })
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

    void console_t::tui_t::update()
    {
        applicable_instructions().update();
        machines().update();

        return;
    }

    void console_t::tui_t::loop() { screen().Loop(component()); }

    void console_t::tui_t::exit_loop() { screen().Exit(); }

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
                std::cerr << "In Machine::console_t::tui_t::width(index_t) const:\n"
                    "Invalid argument.\n" << std::flush;
                std::abort();
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
                std::cerr << "In Machine::console_t::tui_t::height(index_t) const:\n"
                    "Invalid argument.\n" << std::flush;
                std::abort();
        }
    }

    ftxui::ScreenInteractive &console_t::tui_t::screen() { return tui_t::screen_; }

    void console_t::worker_t::add(std::shared_ptr<machine_t> arg)
    {
        std::lock_guard<std::mutex> lock{mutex_};

        list_.emplace_back(std::move(arg));

        return;
    }

    void console_t::worker_t::add(std::list<std::shared_ptr<machine_t>> arg)
    {
        std::lock_guard<std::mutex> lock{mutex_};

        list_.splice(std::cend(list_), arg);

        return;
    }

    console_t::worker_t::worker_t(const console_t *console) : console_{console} {}

    void console_t::worker_t::pause() { pause_ = true; }

    bool console_t::worker_t::finished() { return finished_; }

    void console_t::worker_t::step(std::shared_ptr<machine_t> &arg, bool add_to_console)
    {
        if (not arg)
            return;

        if (arg->state() == machine_t::machine_state_t::running)
        {
            ++console().instruction_counter();
            arg->next();
        }

        else if (console().split() and
            arg->state() == machine_t::machine_state_t::non_deterministic_decision)
        {
            index_t n = arg->n_applicable_instructions();

            for (index_t i = 0; i != n; ++i)
            {
                std::shared_ptr<machine_t> a = std::make_shared<machine_t>(*arg);

                a.select_instruction(i);
                a->next();
                ++console().instruction_counter();

                if (add_to_console)
                    console().add(std::move(a));
                else
                    list_.emplace_back(std::move(a));
            }
        }
    }

    void console_t::worker_t::thread_function()
    {
        bool break_signal = false;

        while (not break_signal)
        {
            std::unique_lock<std::mutex> lock(console().mutex());

            console().cv().wait(lock, []{ return console().request() != request_t::wait; });
            
            switch(console().request())
            {
                case request_t::step :
                {
                    std::shared_ptr<machine_t> m = console().focus();

                    if (m)
                        step(m, true);

                    break;
                }

                case request_t::run :
                {
                    std::shared_ptr<machine_t> m = console().focus();

                    pause_ = false;

                    if (m)
                        while (not pause_)
                            step(m, true);

                    break;
                }

                case request_t::step_all :
                {
                    lock.release();

                    std::lock_guard<std::mutex> lock{mutex_};

                    for (std::shared_ptr<machine_t> &i : list_)
                        step(i, false);

                    break;
                }

                case request_t::run_all
                {
                    lock.release();
                    
                    std::lock_guard<std::mutex> lock{mutex_};

                    pause_ = false;

                    while (not pause_)
                        for (std::shared_ptr<machine_t> &i : list_)
                            step(i, false);

                    break;
                }

                case request_t::break_signal
                {
                    lock.release();

                    break_signal = true;

                    break;
                }
            }

            finished_ = true;

            console().cv_finished().notify_all();
        }

        return;
    }

    void console_t::load_program()
    {
        bool error_in_loading = true;

        std::string message;
        try
        {
            std::ifstream stream{*program_name()};
            std::shared_ptr<machine_t> new_machine = std::make_shared<machine_t>(stream);

            error_in_loading = false;
            message = "Program loaded.";

            machines_ = {std::move(new_machine)};
            focus_ = machines_.front();

            state_ = static_cast<console_state_t>(console_state_t::program_loaded |
                focus_->deterministic());
        }

        catch (std::runtime_error &)
        {
            message = "Error loading the program.";
            if (error_in_loading)
            {
                *tui().menu().strings()[0] = message;
                return;
            }

            machines_.clear();
            focus_ = nullptr;
            state_ = console_state_t::empty;
        }

        tui().devices().input_strings().clear();
        strings_.resize(2);
        strings_[0]->clear();
        strings_[1]->clear();
        tui().menu().strings().resize(3);

        if (focus_)
            for (index_t i = 0; i != std::size(focus_->devices()) + 2; ++i)
            {
                tui().devices().input_strings().emplace_back(std::make_shared<std::string>());
                strings_.emplace_back(std::make_shared<std::string>());
                tui().menu().strings().emplace_back(strings_.back());
            }

        instruction_counter_ = 0;
        *tui().menu().strings()[0] = message;

        tui().update_all();

        return;
    }

    void console_t::initialise_all()
    {
        if (std::empty(machines_))
            throw std::runtime_error{"In Machine::console_t::initialise_all():\n"
                "Console is empty.\n"};

        std::shared_ptr<machine_t> temp = std::move(focus_);
        machines_.clear();
        machines_.emplace_back(std::move(temp));

        focus_ = machines_.front();
        focus_->initialise(*initialiser_string());

        for (index_t i = 0; i != std::size(focus_->devices()); ++i)
            *(tui().devices().input_strings()[i]) = *initialiser_string();

        state_ = static_cast<console_state_t>(focus_->deterministic() |
                console_state_t::program_loaded | console_state_t::initialised);

        instruction_counter_ = 0;

        tui().update();

        return;
    }

    void console_t::initialise_individually()
    {
        if (std::empty(machines_))
            throw std::runtime_error{"In Machine::console_t::initialise_individually():\n"
                "Console is empty.\n"};

        std::shared_ptr<machine_t> temp = std::move(focus_);
        machines_.clear();
        machines_.emplace_back(std::move(temp));

        focus_ = machines_.front();

        std::vector<std::string> input;
        for (index_t i = 0; i != std::size(focus_->devices()); ++i)
        {
            std::shared_ptr<std::string> s = initialiser_string_vector(i);
            input.emplace_back(*s);
            *(tui().devices().input_strings()[i]) = *s;
        }


        focus_->initialise(std::span<const std::string>{std::cbegin(input), std::cend(input)});

        state_ = static_cast<console_state_t>(focus_->deterministic() |
                console_state_t::program_loaded | console_state_t::initialised);
        instruction_counter_ = 0;

        tui().update();

        return;
    }

    void console_t::step()
    {
        workers_.front().step(focus);

        return;
    }

    void console_t::run()
    {
        while (focus_ and focus_->state() == machine_t::machine_state_t::running)
            step();

        tui().update();

        return;
    }

    void console_t::step_all() { return; }

    void console_t::run_all() { return; }

    std::shared_ptr<std::string> console_t::program_name() { return strings_[0]; }

    std::shared_ptr<std::string> console_t::initialiser_string() { return strings_[1]; }
    
    std::shared_ptr<std::string> console_t::initialiser_string_vector(index_t arg)
        { return strings_[2 + arg]; }

    void console_t::reset()
    {
        machines_.clear();
        focus_ = nullptr;
        state_ = console_state_t::empty;
        strings_ = {std::make_shared<std::string>(), std::make_shared<std::string>()};
        instruction_counter_ = 0;

        tui().update_all();

        return;
    }

    std::shared_ptr<machine_t> console_t::focus() { return focus_; }

    console_t::tui_t &console_t::tui() { return tui_; }

    void console_t::tui_loop() { tui_.loop(); } 
}

