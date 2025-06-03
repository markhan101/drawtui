cmake_minimum_required(VERSION 3.10)
project(drawtui)
set(CMAKE_CXX_STANDARD 17)

# Fetch FTXUI from GitHub
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0  # Use a specific version for reproducibility
)

# Make FTXUI available
FetchContent_MakeAvailable(ftxui)

add_executable(drawtui main.cpp)

target_link_libraries(drawtui 
  ftxui::screen 
  ftxui::dom 
  ftxui::component
)