#include <iostream>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/screen/color.hpp"

int main()
{
    using namespace ftxui;

    int mouse_x = 0;
    int mouse_y = 0;
    bool is_drawing = false;

    std::vector<std::pair<std::pair<int, int>, int>> clicked_points; //{(x,y), 0} coordinates and choice number

    Component simple_pen = Renderer([&]
                                    {
    Canvas c = Canvas(300, 300);
    c.DrawText(0, 0, "Simple pen");
    for (size_t i = 0; i < clicked_points.size(); i++)
    {
        if(clicked_points[i].second == 0)
        c.DrawPoint(clicked_points[i].first.first, clicked_points[i].first.second, true,Color::Blue3);
        else if (clicked_points[i].second == 1)
        c.DrawBlockCircle(clicked_points[i].first.first, clicked_points[i].first.second, true,Color::Blue3);
    }
    return canvas(std::move(c)); });

    Component thick_pen = Renderer([&]
                                   {
    Canvas c = Canvas(300, 300);
    c.DrawText(0, 0, "Thick pen");
    for (size_t i = 0; i < clicked_points.size(); i++)
    {
        if(clicked_points[i].second == 1)
        c.DrawBlockCircle(clicked_points[i].first.first, clicked_points[i].first.second, true,Color::Blue3);
        else if(clicked_points[i].second == 0)
        c.DrawPoint(clicked_points[i].first.first, clicked_points[i].first.second, true,Color::Blue3);
    }
    return canvas(std::move(c)); });

    Component clear_board = Renderer([&]
                                     {
                                         Canvas c = Canvas(300, 300);
                                         c.DrawText(0, 0, "Board Cleared!");
                                         clicked_points.clear();
                                         return canvas(std::move(c)); });

    int selected_tab = 0;
    Component tab = Container::Tab(
        {simple_pen,
         thick_pen,
         clear_board},
        &selected_tab);

    Component tab_with_mouse = CatchEvent(tab, [&](Event e)
                                          {
        if (e.is_mouse() && e.mouse().x < 130)
        {
            

            mouse_x = (e.mouse().x - 1) * 2;
            mouse_y = (e.mouse().y - 1) * 4;

            if (e.mouse().button == Mouse::Button::Left)
            {
                if (e.mouse().motion == Mouse::Motion::Pressed)
                {
                    is_drawing = true;
                    if(selected_tab == 0)
                    clicked_points.push_back({{mouse_x, mouse_y},0});
                    else if (selected_tab == 1)
                    clicked_points.push_back({{mouse_x, mouse_y},1});
                    return true;
                }
                else if (e.mouse().motion == Mouse::Motion::Released)
                {
                    is_drawing = false;
                    return true;
                }
            }
            if (is_drawing && e.mouse().motion == Mouse::Motion::Pressed)
            {
                if(selected_tab == 0)
                    clicked_points.push_back({{mouse_x, mouse_y},0});
                else if (selected_tab == 1)
                    clicked_points.push_back({{mouse_x, mouse_y},1});
                return true;
            }
        
        }
    return false; });

    std::vector<std::string> tab_titles = {
        "Simple Pen",
        "Thick Pen",
        "Clear Board"};
    Component tab_toggle = Menu(&tab_titles, &selected_tab);

    Component component = Container::Horizontal({tab_toggle,
                                                 tab_with_mouse});

    Component component_renderer = Renderer(component, [&]
                                            { return hbox({tab_with_mouse->Render(),
                                                           separator(),
                                                           tab_toggle->Render() | size(WIDTH, EQUAL, 30)}) |
                                                     border; });

    ScreenInteractive screen = ScreenInteractive::FitComponent();
    screen.Loop(component_renderer);

    return 0;
}