cmake_minimum_required(VERSION 3.14)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(MyApp
    sources/main.cpp
    sources/app_settings.cpp
    sources/menu.cpp
    sources/connection_test.cpp
    sources/resource_test.cpp
    sources/menu_item.h
    sources/data_pool.h
)

