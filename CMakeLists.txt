cmake_minimum_required(VERSION 3.16)

project(arser LANGUAGES CXX)

add_library(arser INTERFACE)

# it specifies INTERFACE so that future targets link with arser library will inherit its include directory.
# it means that a developer who want to link arser just need to add one line
# target_link_library(another-users-target arser) 
target_include_directories(arser INTERFACE include/)

# gtest
enable_testing()
include(GoogleTest)
mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} gtest gtest_main pthread)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()


add_subdirectory(tests)
