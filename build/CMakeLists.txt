cmake_minimum_required(VERSION 4.1.1)

project(yatta VERSION 1.0)
find_package(SQLite3 REQUIRED)

add_executable(yatta src/main.cpp)
target_link_libraries(yatta PRIVATE SQLite::SQLite3)