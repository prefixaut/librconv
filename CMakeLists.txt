# CMakeList.txt : CMake project for librconv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("librconv" VERSION 0.1)

# Set C-Version to 17
if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD 17)
endif()

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
# add_library(fmem "lib/fmem")

# Disable windows warnings to use secure (_s) functions instead
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# In case we do use the _s functions, enable them
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
