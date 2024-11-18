cmake_minimum_required(VERSION 3.22)

project(
    stella_garbage_collector
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(
    ${PROJECT_NAME}
    stella/gc.c
    stella/runtime.c
    main.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -DSTELLA_GC_STATS -DSTELLA_RUNTIME_STATS)
