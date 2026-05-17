cmake_minimum_required(VERSION 3.12)
project(step-2-MMN VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Ищем пакет OpenMP в системе
find_package(OpenMP REQUIRED)

set(SOURCES
    src/Source.cpp
    src/DirihleVPuassone.cpp
)

set(HEADERS
    header/DirihleVPuassone.h
)

add_executable(calculate ${SOURCES} ${HEADERS})

target_include_directories(calculate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)

# 2. Подключаем OpenMP к таргет-цели
target_link_libraries(calculate PRIVATE OpenMP::OpenMP_CXX)

# 3. Специфичные флаги для компиляторов
if(MSVC)
    target_compile_definitions(calculate PRIVATE _CRT_SECURE_NO_WARNINGS)
    # /openmp:llvm включает современный бэкенд OpenMP для поддержки редукций по типу max/min
    target_compile_options(calculate PRIVATE /W4 /EHsc /openmp:llvm)
else()
    target_compile_options(calculate PRIVATE -Wall -Wextra -pedantic)
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
        target_link_libraries(calculate PRIVATE stdc++fs)
    endif()
endif()