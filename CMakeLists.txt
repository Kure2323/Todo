cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
PROJECT(todo)

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG 3eda91b
)
FetchContent_MakeAvailable(argparse)

add_executable(todo)
target_link_libraries(todo argparse)

target_sources(todo
  PRIVATE
  src/main.cpp
)

install(TARGETS todo
    RUNTIME DESTINATION bin
)
