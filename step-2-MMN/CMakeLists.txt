cmake_minimum_required(VERSION 3.12)
project(Ступень_2_ММН VERSION 1.0.0 LANGUAGES CXX)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Указываем пути к исходным файлам с учетом папки src
set(SOURCES
    src/Source.cpp
    src/DirihleVPuassone.cpp
)

# Указываем путь к заголовочным файлам с учетом папки header
set(HEADERS
    header/DirihleVPuassone.h
)

# Создаем цель с именем calculate
add_executable(calculate ${SOURCES} ${HEADERS})

# Настройка путей к заголовочным файлам
target_include_directories(calculate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)

# Настройки компилятора
if(MSVC)
    target_compile_definitions(calculate PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(calculate PRIVATE /W4 /EHsc)
else()
    target_compile_options(calculate PRIVATE -Wall -Wextra -pedantic)
    
    # Линковка с stdc++fs для старых версий GCC
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
        target_link_libraries(calculate PRIVATE stdc++fs)
    endif()
endif()