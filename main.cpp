
#include <cmath>
#include <ftxui/dom/elements.hpp>                 // for canvas, Element, separator, hbox, operator|, border
#include <ftxui/screen/screen.hpp>                // for Pixel
#include <memory>                                 // for allocator, shared_ptr, __shared_ptr_access
#include <string>                                 // for string, basic_string
#include <utility>                                // for move
#include <vector>                                 // for vector, __alloc_traits<>::value_type
#include "ftxui/component/component.hpp"          // for Renderer, CatchEvent, Horizontal, Menu, Tab
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/event.hpp"              // for Event
#include "ftxui/component/mouse.hpp"              // for Mouse
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/canvas.hpp"                   // for Canvas
#include "ftxui/screen/color.hpp"

int main()
{
    using namespace ftxui;

    int mouse_x = 0;
    int mouse_y = 0;
    bool is_drawing = false;

    std::vector<std::pair<int, int>> clicked_points;

    auto simple_pen = Renderer([&]
                               {
    auto c = Canvas(300, 300);
    c.DrawText(0, 0, "Simple pen");
    for (size_t i = 0; i < clicked_points.size(); i++)
    {
        c.DrawPoint(clicked_points[i].first, clicked_points[i].second, true,Color::Blue3);
    }
    return canvas(std::move(c)); });

    auto thick_pen = Renderer([&]
                              {
    auto c = Canvas(300, 300);
    c.DrawText(0, 0, "Thick pen");
    for (size_t i = 0; i < clicked_points.size(); i++)
    {
        c.DrawBlockCircle(clicked_points[i].first, clicked_points[i].second, true,Color::Blue3);
    }
    return canvas(std::move(c)); });

    int selected_tab = 1;
    auto tab = Container::Tab(
        {
            simple_pen,
        },
        &selected_tab);

    auto tab_with_mouse = CatchEvent(tab, [&](Event e)
                                     {
        if (e.is_mouse())
        {

            mouse_x = (e.mouse().x - 1) * 2;
            mouse_y = (e.mouse().y - 1) * 4;

           if (e.mouse().button == Mouse::Button::Left) 
        {
                if (e.mouse().motion == Mouse::Motion::Pressed ) 
                {
                    is_drawing = true;
                    clicked_points.push_back({mouse_x, mouse_y});
                    return true;
                }
                else if (e.mouse().motion == Mouse::Motion::Released) 
                {
                    is_drawing = false;
                    return true;
                }
        } 
            if (is_drawing && e.mouse().motion == Mouse::Motion::Moved) {
            clicked_points.push_back({mouse_x, mouse_y});
            return true;
    }
        }
    return false; });

    std::vector<std::string> tab_titles = {
        "Simple Pen",
        "Thick Pen"};
    auto tab_toggle = Menu(&tab_titles, &selected_tab);

    auto component = Container::Horizontal({tab_toggle,
                                            tab_with_mouse});

    auto component_renderer = Renderer(component, [&]
                                       { return hbox({
                                                    tab_with_mouse->Render(),
                                                    separator(),
                                                    tab_toggle->Render(),
                                                }) |
                                                border; });

    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(component_renderer);

    return 0;
}