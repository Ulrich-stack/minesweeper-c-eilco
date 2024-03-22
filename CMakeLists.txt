cmake_minimum_required(VERSION 3.26)
project(minesweeper_c_eilco C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(include)

add_executable(minesweeper_c_eilco
        include/game.h
        src/game.c
        main.c
        src/database_management.c
        src/interface.c
)
