# CMakeList.txt : CMake project for librconv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("librconv" VERSION 0.1)

# Add source to this project's executable.
file(GLOB SRC_FILES "src/*.c" "src/**/*.c")
add_executable(librconv ${SRC_FILES})

# Configure the files
configure_file("src/rconv.h.in" "src/rconv.h")

# Include target directories
target_include_directories(librconv PUBLIC "${PROJECT_BINARY_DIR}")

# Libraries
target_include_directories(librconv PUBLIC
  "src"
  "lib/utf8"
)

if(__STD_LIB_EXT1__ EQUAL 1)
  target_compile_definitions(librconv PUBLIC __STDC_WANT_LIB_EXT1__=1)
endif()

add_compile_options(
  -Wall
  -Wextra
  -Wconversion
  -Wsign-conversion
  $<$<CONFIG:DEBUG>:-g3>
  $<$<CONFIG:DEBUG>:-Og>
  $<$<CONFIG:RELEASE>:-O3>
)

# TODO: Add tests and install targets if needed.
