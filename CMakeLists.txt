# CMakeList.txt : CMake project for rconv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("rconv" VERSION 0.1)

# Add source to this project's executable.
file(GLOB SRC_FILES "src/*.c" "src/**/*.c")
add_executable(rconv ${SRC_FILES})

# Configure the files
configure_file("src/rconv.h.in" "src/rconv.h")

# Include target directories
target_include_directories(rconv PUBLIC "${PROJECT_BINARY_DIR}")

# Libraries
target_include_directories(rconv PUBLIC
  "src"
  "lib/utf8"
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_include_directories(rconv PUBLIC "lib/mpdecimal/vcbuild/dist64")
    target_link_libraries(rconv "lib/mpdecimal/vcbuild/dist64/libmpdec-2.5.1.lib")
  else()
    target_include_directories(rconv PUBLIC "lib/mpdecimal/vcbuild/dist32")
    target_link_libraries(rconv "lib/mpdecimal/vcbuild/dist32/libmpdec-2.5.1.lib")
  endif()
else()
  target_include_directories(rconv PUBLIC "lib/mpdecimal/libmpdec")
  target_link_libraries(rconv "lib/mpdecimal/libmpdec/libmpdec-2.5.1.a")
endif()

if(__STD_LIB_EXT1__ EQUAL 1)
  target_compile_definitions(rconv PUBLIC __STDC_WANT_LIB_EXT1__=1)
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
