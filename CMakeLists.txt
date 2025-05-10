cmake_minimum_required(VERSION 3.10)
project(PlatformFighter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add executable
add_executable(game 
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Platform.cpp
    src/Enemy.cpp
)

# Include directories
target_include_directories(game PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    include
)

# Link libraries
target_link_libraries(game PRIVATE 
    ${SDL2_LIBRARIES}
)
