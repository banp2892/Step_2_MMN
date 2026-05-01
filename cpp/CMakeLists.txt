cmake_minimum_required(VERSION 3.12)
project(step-2-MMN VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/Source.cpp
    src/DirihleVPuassone.cpp
)

set(HEADERS
    header/DirihleVPuassone.h
)

add_executable(calculate ${SOURCES} ${HEADERS})

target_include_directories(calculate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)

if(MSVC)
    target_compile_definitions(calculate PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(calculate PRIVATE /W4 /EHsc)
else()
    target_compile_options(calculate PRIVATE -Wall -Wextra -pedantic)
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
        target_link_libraries(calculate PRIVATE stdc++fs)
    endif()
endif()