# CMakeList.txt : CMake project for librconv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(PROJECT_NAME rconv)
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C)

# Disable windows warnings to use secure (_s) functions instead
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W3")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

foreach(flag ${CHECK_C_FLAGS})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
endforeach()

#set(CMAKE_DL_LIBS "${CMAKE_DL_LIBS} check m")

enable_testing()

# Set C-Version to 17
if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD 17)
endif()

# Add source to this project's executable.
file(GLOB SRC_FILES "src/*.c" "src/**/*.c")
add_library(${PROJECT_NAME} ${SRC_FILES})

# Add all libraries to the linker
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

# Configure the files
# configure_file("src/rconv.h.in" "src/rconv.h")

# Include target directories
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

# Libraries
target_include_directories(${PROJECT_NAME} PUBLIC
  "src"
  "lib/utf8"
)

# Add Tests 
add_subdirectory(tests)

# In case we do use the _s functions, enable them
if(__STD_LIB_EXT1__ EQUAL 1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC __STDC_WANT_LIB_EXT1__=1)
endif()
