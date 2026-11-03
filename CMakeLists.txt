cmake_minimum_required(VERSION 3.20)

project(LibraryManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Download the Crow C++ web framework
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1
)

set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(Crow)

# Find SQLite database library
find_package(SQLite3 REQUIRED)

# Original C++ console application
add_executable(library main.cpp)

# C++ web application
add_executable(library_server server.cpp)

target_link_libraries(
    library_server
    PRIVATE
    Crow::Crow
    SQLite::SQLite3
)

# Copy website interface into the build directory
configure_file(
    ${CMAKE_SOURCE_DIR}/index.html
    ${CMAKE_BINARY_DIR}/index.html
    COPYONLY
)
