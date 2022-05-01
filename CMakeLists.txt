# CMakeList.txt : CMake project for rconv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("rconv" VERSION 0.1)

# Add source to this project's executable.
add_executable (rconv "src/rconv.c" "src/rconv.h")

# Configure the files
configure_file("src/rconv.h.in" "src/rconv.h")

# Include target directories
target_include_directories(rconv PUBLIC "${PROJECT_BINARY_DIR}")

# Libraries
target_link_libraries(rconv PUBLIC include)

# TODO: Add tests and install targets if needed.
