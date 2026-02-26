cmake_minimum_required(VERSION 3.16)
project(SearchEngine_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "*.cpp")
file(GLOB PROGRAM_SOURCES "../SearchEngine/*.cpp")
list(REMOVE_ITEM PROGRAM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../SearchEngine/main.cpp")

add_executable(${PROJECT_NAME} 
    ${TEST_SOURCES}
    ${PROGRAM_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../SearchEngine
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})