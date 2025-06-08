#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <math.h>

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

#define CANVAS_WIDTH 300
#define CANVAS_HEIGHT 300

ftxui::Element ColorTile(int red, int green, int blue)
{
  return ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 14) |
         ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 7) | ftxui::bgcolor(ftxui::Color::RGB(red, green, blue));
}

ftxui::Element ColorString(int red, int green, int blue)
{
  return ftxui::text("RGB = (" +
                     std::to_string(red) + "," +
                     std::to_string(green) + "," +
                     std::to_string(blue) + ")");
}

void set_default_colours(int selection, int *red, int *green, int *blue)
{
  switch (selection)
  {
  case 0:
    *red = 255;
    *green = 0;
    *blue = 0;
    break;
  case 1:
    *red = 0;
    *green = 0;
    *blue = 255;
    break;
  case 2:
    *red = 0;
    *green = 255;
    *blue = 0;
    break;
  case 3:
    *red = 255;
    *green = 255;
    *blue = 0;
    break;
  case 4:
    *red = 255;
    *green = 165;
    *blue = 0;
    break;
  case 5:
    *red = 255;
    *green = 255;
    *blue = 255;
    break;
  default:
    break;
  }
}

class Clicked_Point
{
private:
  std::pair<int,
            int>
      coord;
  int tab_option;
  int red,
      green,
      blue;

public:
  Clicked_Point(std::pair<int, int> c, int t_o, int r = 50, int g = 90, int b = 200) : coord(c),
                                                                                       tab_option(t_o),
                                                                                       red(r),
                                                                                       green(g),
                                                                                       blue(b) {}

  int get_tab_option() const
  {
    return tab_option;
  }

  int get_red() const
  {
    return red;
  }
  int get_green() const
  {
    return green;
  }
  int get_blue() const
  {
    return blue;
  }

  int get_x() const
  {
    return coord.first;
  }
  int get_y() const
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

  int red = 255;
  int green = 0;
  int blue = 0;

  int selected_tab = 0;
  int prev_selected_tab = 0;

  int radio_selected = 0;
  int prev_radio_selected = 0;
  int selected_pen = 0; // value goes to 10 or 11
  int prev_selected_pen = 0;
  int selected_brush = 0; // values goest to 20 or 21
  int prev_selected_brush = 0;
  int currently_active_tab = 10; // default for pen x1
  int last_changed = 1;          // 1 for pen change 2 for brush change 3 for eraser and 4 for clear board

  std::vector<Clicked_Point>
      cp;

  Component simple_pen = Renderer([&]
                                  {
    Canvas c = Canvas(CANVAS_WIDTH, CANVAS_HEIGHT);
    c.DrawText(0, 0, "Simple Pen 1x");
    for (size_t i = 0; i < cp.size(); i++) {
      if (cp[i].get_tab_option() == 0)
        c.DrawPoint(cp[i].get_x(), cp[i].get_y(), true, Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
      else if (cp[i].get_tab_option() == 1)
        c.DrawBlockCircle(cp[i].get_x(), cp[i].get_y(), true, Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
    }
    return canvas(std::move(c)); });

  Component thick_pen = Renderer([&]
                                 {
    Canvas c = Canvas(CANVAS_WIDTH, CANVAS_HEIGHT);
    c.DrawText(0, 0, std::to_string(selected_tab));
    for (size_t i = 0; i < cp.size(); i++) {
      if (cp[i].get_tab_option() == 0)
        c.DrawPoint(cp[i].get_x(), cp[i].get_y(), true, Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
      else if (cp[i].get_tab_option() == 1)
        c.DrawBlockCircle(cp[i].get_x(), cp[i].get_y(), true, Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
    }
    return canvas(std::move(c)); });

  Component clear_board = Renderer([&]
                                   {
Canvas c = Canvas(CANVAS_WIDTH, CANVAS_HEIGHT);
    c.DrawText(0, 0, "Board Cleared!");
    cp.clear();
    return canvas(std::move(c)); });

  Component eraser = Renderer([&]
                              {

          
Canvas c = Canvas(CANVAS_WIDTH, CANVAS_HEIGHT);
    c.DrawText(0,0,"Eraser");
    for (size_t i = 0; i < cp.size(); i++) {

      if (cp[i].get_tab_option() == 0)
        c.DrawPoint(cp[i].get_x(), cp[i].get_y(), true, Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));
      else if (cp[i].get_tab_option() == 1)
        c.DrawBlockCircle(cp[i].get_x(), cp[i].get_y(), true, Color::RGB(cp[i].get_red(), cp[i].get_green(), cp[i].get_blue()));

    }
    return canvas(std::move(c)); });

  Component tab = Container::Tab({simple_pen, thick_pen, eraser,
                                  clear_board},
                                 &selected_tab);

  Component tab_with_mouse = CatchEvent(tab, [&](Event e)
                                        {            
    if (e.is_mouse() && e.mouse().x < int(std::floor(Terminal::Size().dimx / 1.177))) // Dynamic for terminal 1.17 is ratio of total col/canvas end(on terminal)
    {
      mouse_x = (e.mouse().x - 1) * 2;
      mouse_y = (e.mouse().y - 1) * 4;

      if (e.mouse().button == Mouse::Button::Left) {
        if (e.mouse().motion == Mouse::Motion::Pressed) {
          is_drawing = true;
          if (currently_active_tab == 10)
            cp.push_back({
              {
                mouse_x,
                mouse_y
              },
              0,
              red,
              green,
              blue
            });
          else if (currently_active_tab == 20)
            cp.push_back({
              {
                mouse_x,
                mouse_y
              },
              1,
              red,
              green,
              blue
            });
          else if (currently_active_tab == 99) {

            auto it = std::find_if(cp.begin(), cp.end(),
              [mouse_x, mouse_y](const Clicked_Point & point) {
                return point.get_x() == mouse_x && point.get_y() == mouse_y;
              });

            if (it != cp.end()) {
              cp.erase(it);

            }
          }
          else if (currently_active_tab == 100)
          {
            cp.clear();
          }
          return true;
        } else if (e.mouse().motion == Mouse::Motion::Released) {
          is_drawing = false;
          return true;
        }
      }
      if (is_drawing && e.mouse().motion == Mouse::Motion::Pressed) {
        if (selected_tab == 0)
          cp.push_back({
            {
              mouse_x,
              mouse_y
            },
            0,
            red,
            green,
            blue
          });
        else if (selected_tab == 1)
          cp.push_back({
            {
              mouse_x,
              mouse_y
            },
            1,
            red,
            green,
            blue
          });
        else if (selected_tab == 2) {

          //get iterator for the point
          auto it = std::find_if(cp.begin(), cp.end(),
            [mouse_x, mouse_y](const Clicked_Point & point) {
              return point.get_x() == mouse_x && point.get_y() == mouse_y;
            });

          //remove point
          if (it != cp.end()) {
            cp.erase(it);

          }

        }
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
      "Eraser",
      "Clear Board"};

  std::vector<std::string> pen_types = {
      "Pen 1x", // 1x default
      "Pen 2x"};

  std::vector<std::string>
      brush_types = {"Brush 1x", "Brush 2x"};

  std::vector<std::string>
      radio_colour_options = {"Red", "Blue", "Green", "Yellow", "Orange", "White"};

  Component radiobox_colours = Radiobox(&radio_colour_options, &radio_selected);

  Component tab_toggle = Menu(&tab_titles, &selected_tab);

  Component pen_toggle = Dropdown(&pen_types, &selected_pen);
  Component brush_toggle = Dropdown(&brush_types, &selected_brush);

  Component toggle_container = Container::Horizontal({pen_toggle,
                                                      brush_toggle});

  Component toggle_box = Container::Vertical({toggle_container, tab_toggle});

  Component main_component = Container::Horizontal({toggle_box,
                                                    tab_with_mouse,
                                                    radiobox_colours,
                                                    slider_container});

  Component component_renderer = Renderer(main_component, [&]
                                          {

    if (radio_selected != prev_radio_selected) {
      set_default_colours(radio_selected, & red, & green, & blue);
      prev_radio_selected = radio_selected;
    }

    if (selected_pen != prev_selected_pen) 
        {
            currently_active_tab = selected_pen + 10; 
            last_changed = 1;
            prev_selected_pen = selected_pen;
        }
        else if (selected_brush != prev_selected_brush) {
            currently_active_tab = selected_brush + 20; 
            last_changed = 2;
            prev_selected_brush = selected_brush;
        }
        else if (selected_tab != prev_selected_tab)
        {
          if (selected_tab == 0)
          {
              currently_active_tab = 0; // Eraser mode
              last_changed = 3;
          }
          else if (selected_tab == 1)
          {
              currently_active_tab = 100; // Clear board mode
              last_changed = 4;
          }
          prev_selected_tab = selected_tab;
        }

    return hbox({
        tab_with_mouse -> Render(),
        separator(),
        vbox({
          toggle_box -> Render(),
          separator(),
          ColorTile(red, green, blue),
          separator(),
          radiobox_colours -> Render(),
          separator(),
          slider_red -> Render(),
          separator(),
          slider_green -> Render(),
          separator(),
          slider_blue -> Render(),
          separator(),
          ColorString(red, green, blue),
        }) | size(WIDTH, EQUAL, 30),
      }) |
      border; });
  ScreenInteractive screen = ScreenInteractive::FitComponent();
  screen.Loop(component_renderer);

  return 0;
}