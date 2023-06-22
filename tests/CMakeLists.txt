include(CheckCSourceCompiles)
include(CheckLibraryExists)
find_package(PkgConfig)

pkg_search_module(CHECK check)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

if(CHECK_FOUND)
    if(CHECK_VERSION VERSION_LESS 0.10.0)
        MESSAGE( "Check version older than 0.10.0" )
        set(CHECK_FOUND 0)
    endif()
else()
    find_path(CHECK_INCLUDE_DIRS check.h)
    find_library(CHECK_LIBRARIES NAMES check)
    if(CHECK_INCLUDE_DIRS AND CHECK_LIBRARIES)
        set(CHECK_CFLAGS)
        set(CHECK_LIBRARY_DIRS)
        set(CHECK_FOUND 1)
    endif()
endif()

if(CHECK_FOUND)
    enable_testing()

    include_directories("${CMAKE_SOURCE_DIR}/src")
    include_directories("${CMAKE_SOURCE_DIR}/tests")
    include_directories("${CMAKE_SOURCE_DIR}/lib/utf8")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

    include_directories(${CHECK_INCLUDE_DIRS})
    link_directories(${CHECK_LIBRARY_DIRS})

    set(TEST_SOURCES
        check_common
        check_data_descriptor
        check_decimals
        check_stepmania
    )

    foreach(test_file ${TEST_SOURCES})
        add_executable("${test_file}" "${test_file}.c")
        target_link_libraries("${test_file}" ${PROJECT_NAME} check m subunit)
        add_test("${test_file}" "${CMAKE_CURRENT_BINARY_DIR}/${test_file}")
    endforeach()

ELSE(CHECK_FOUND)

    MESSAGE("Tests disabled because Check was not found")

ENDIF(CHECK_FOUND)
