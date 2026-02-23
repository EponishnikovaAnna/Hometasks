cmake_minimum_required(VERSION 3.6)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)  
include_directories("includes/")  

set(SOURCE_LIST sources/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_LIST})