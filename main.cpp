#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/component/captured_mouse.hpp"

using namespace ftxui;

Element ColorTile(int red, int green, int blue)
{
    return text("") | size(WIDTH, GREATER_THAN, 14) |
           size(HEIGHT, GREATER_THAN, 7) | bgcolor(Color::RGB(red, green, blue));
}

Element ColorString(int red, int green, int blue)
{
    return text("RGB = (" +                   //
                std::to_string(red) + "," +   //
                std::to_string(green) + "," + //
                std::to_string(blue) + ")"    //
    );
}

class Clicked_Point
{
private:
    std::pair<int, int> coord;
    int tab_option;
    int red, green, blue;

public:
    Clicked_Point(std::pair<int, int> c, int t_o, int r = 50, int g = 90, int b = 200) : coord(c), tab_option(t_o), red(r), green(g), blue(b)
    {
    }

    int get_tab_option()
    {
        return tab_option;
    }

    int get_red()
    {
        return red;
    }
    int get_green()
    {
        return green;
    }
    int get_blue()
    {
        return blue;
    }

    int get_x()
    {
        return coord.first;
    }
    int get_y()
    {
        return coord.second;
    }
};

int main()
{
    using namespace ftxui;

    int mouse_x = 0;
    int mouse_y = 0;
    bool is_drawing = false;

    int red = 128;
    int green = 25;
    int blue = 100;

    std::vector<std::pair<std::pair<int, int>, int>> clicked_points; //{(x,y), 0} coordinates and choice number
    std::vector<Clicked_Point> cp;

    Component simple_pen = Renderer([&]
                                    {
    Canvas c = Canvas(300, 300);
    c.DrawText(0, 0, "Simple pen");
    for (size_t i = 0; i < cp.size(); i++)
    {
        if(cp[i].get_tab_option() == 0)
        c.DrawPoint(cp[i].get_x(), cp[i].get_y(), true,Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
        
        else if (cp[i].get_tab_option() == 1)
        c.DrawBlockCircle(cp[i].get_x(), cp[i].get_y(), true,Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
    }
    return canvas(std::move(c)); });

    Component thick_pen = Renderer([&]
                                   {
    Canvas c = Canvas(300, 300);
    c.DrawText(0, 0, "Thick pen");
    for (size_t i = 0; i < cp.size(); i++)
    {
        if(cp[i].get_tab_option() == 0)
        c.DrawPoint(cp[i].get_x(), cp[i].get_y(), true,Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
        else if (cp[i].get_tab_option() == 1)
        c.DrawBlockCircle(cp[i].get_x(), cp[i].get_y(), true,Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
    }
    return canvas(std::move(c)); });

    Component clear_board = Renderer([&]
                                     {
                                         Canvas c = Canvas(300, 300);
                                         c.DrawText(0, 0, "Board Cleared!");
                                         cp.clear();
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
                    cp.push_back({{mouse_x,mouse_y},0,red, green, blue});
                    else if (selected_tab == 1)
                    cp.push_back({{mouse_x, mouse_y},1,red, green, blue});
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
                    cp.push_back({{mouse_x,mouse_y},0,red, green, blue});
                else if (selected_tab == 1)
                    cp.push_back({{mouse_x, mouse_y},1,red, green, blue});
                return true;
            }

        }
    return false; });

    Component slider_red = Slider("Red  :", &red, 0, 255, 1);
    Component slider_green = Slider("Green:", &green, 0, 255, 1);
    Component slider_blue = Slider("Blue :", &blue, 0, 255, 1);

    Component slider_container = Container::Vertical({
        slider_red,
        slider_green,
        slider_blue,
    });

    std::vector<std::string> tab_titles = {
        "Simple Pen",
        "Thick Pen",
        "Clear Board"};
    Component tab_toggle = Menu(&tab_titles, &selected_tab);

    Component main_component = Container::Horizontal({tab_toggle,
                                                      tab_with_mouse, slider_container});

    Component component_renderer = Renderer(main_component, [&]
                                            { return hbox({
                                                         tab_with_mouse->Render(),
                                                         separator(),
                                                         vbox({
                                                             tab_toggle->Render(),
                                                             separator(),
                                                             ColorTile(red, green, blue),
                                                             separator(),
                                                             ColorString(red, green, blue),
                                                             separator(),
                                                             slider_red->Render(),
                                                             separator(),
                                                             slider_green->Render(),
                                                             separator(),
                                                             slider_blue->Render(),
                                                             separator(),
                                                             ColorString(red, green, blue),
                                                         }) | size(WIDTH, EQUAL, 30),
                                                     }) |
                                                     border; });
    ScreenInteractive screen = ScreenInteractive::FitComponent();
    screen.Loop(component_renderer);

    return 0;
}
