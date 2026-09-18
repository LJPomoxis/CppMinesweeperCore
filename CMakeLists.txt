cmake_minimum_required(VERSION 3.21)
project(core_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core_engine STATIC
    core.cpp
)

target_include_directories(core_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(PROJECT_IS_TOP_LEVEL)
    add_executable(core_engine_test
        main.cpp
    )
    target_link_libraries(core_engine_test PRIVATE core_engine)
endif()