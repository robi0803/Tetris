project                (Tetris)
cmake_minimum_required (VERSION 2.8)
add_executable         (Tetris main.c)
include                (FindPkgConfig)
pkg_search_module      (SDL2 REQUIRED sdl2)
pkg_search_module      (SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
include_directories    (${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries  (Tetris ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
