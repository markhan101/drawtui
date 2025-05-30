cmake_minimum_required(VERSION 3.10)
project(drawtui)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(FTXUI) # Add the FTXUI folder

add_executable(drawtui main.cpp)

target_link_libraries(drawtui ftxui::screen ftxui::dom ftxui::component)