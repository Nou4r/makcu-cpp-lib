cmake_minimum_required(VERSION 3.21)

project(makcu_cpp_lib VERSION 1.0.0 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "makcu-cpp-lib currently supports Windows only.")
endif()

set(MAKCU_PUBLIC_HEADERS
    include/makcu/enums.h
    include/makcu/errors.h
    include/makcu/Mouse.h
    include/makcu/SerialTransport.h
    include/makcu/version.h
    include/makcu/makcu.h
)

set(MAKCU_LIBRARY_SOURCES
    src/Mouse.cpp
    src/SerialTransport.cpp
)

add_library(makcu_cpp_lib STATIC)
add_library(makcu::makcu_cpp_lib ALIAS makcu_cpp_lib)

target_sources(makcu_cpp_lib
    PRIVATE
        ${MAKCU_LIBRARY_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES ${MAKCU_PUBLIC_HEADERS}
)

target_include_directories(makcu_cpp_lib PUBLIC include)
target_compile_features(makcu_cpp_lib PUBLIC cxx_std_20)
target_compile_definitions(makcu_cpp_lib PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)

if(MSVC)
    target_compile_options(makcu_cpp_lib PRIVATE /W4 /permissive-)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAKCU_PUBLIC_HEADERS} ${MAKCU_LIBRARY_SOURCES})

function(makcu_add_example target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE makcu::makcu_cpp_lib)
    target_compile_features(${target_name} PRIVATE cxx_std_20)

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    endif()

    set_target_properties(${target_name} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_file})
endfunction()

makcu_add_example(makcu_example examples/example.cpp)
makcu_add_example(makcu_cli examples/makcu_cli.cpp)
makcu_add_example(makcu_keypad8_circles examples/keypad8_circles.cpp)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT makcu_keypad8_circles)
