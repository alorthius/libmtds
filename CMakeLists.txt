cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
#  It is used as your project's main executable name. 
set(PROJECT_NAME mtds_example)
set(LIB_NAME mtds)
project(${PROJECT_NAME} C CXX) # project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

##########################################################
# User configurable options of the template
##########################################################
# Note: symbols like WARNINGS_AS_ERRORS in configuration are intentionally variables
# and not CMake options --using options creates too much problem for students.

#! It is a good practice to set "WARNINGS_AS_ERRORS" ON,
#  but sometimes it creates too much trouble, so default is OFF.
set(WARNINGS_AS_ERRORS ON)

#! Always use PVS Studio while developing. 
set(ENABLE_PVS_STUDIO OFF)

#! Select appropriate sanitizers.
#  Definitely enable sanitizers while developing.
#  Disable it for the production builds and before submitting for grading.
#  Only one of Memory (MSAN), Address (ASAN), or Thread (TSan)
#  sanitizers is applicable at the time -- the first defined.

#! UndefinedBehaviorSanitizer (UBSan)
#  Info: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
set(ENABLE_UBSan OFF)
#! AddressSanitizer -- detects use after free or after scope exit,
#  memory overflows and leaks.  
#  Info: https://github.com/google/sanitizers/wiki/AddressSanitizer
set(ENABLE_ASAN OFF)
#! ThreadSanitizer -- detects data races.
set(ENABLE_TSan OFF)
#! MemorySanitizer detects uninitialized memory reads 
#  Info: https://github.com/google/sanitizers/wiki/MemorySanitizer
set(ENABLE_MSAN OFF)


#! Be default -- build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

# Warnings as errors should be imported here -- do not move this line
include(cmake/CompilerWarnings.cmake)

##########################################################
# Project files, packages, libraries and so on
##########################################################

#! Project main executable source compilation
add_executable(${PROJECT_NAME} src/main.cpp mtds_lib/mutex_queue.hpp mtds_lib/mpsc_queue.hpp
		mtds_lib/details/tagged_ptr.hpp mtds_lib/mpsc_stack.hpp mtds_lib/mpmc_queue.hpp mtds_lib/mpmc_stack.hpp)

#! Put path to your project headers
target_include_directories(${PROJECT_NAME} PRIVATE mtds_lib)

add_library(
		${LIB_NAME} SHARED
		${CMAKE_SOURCE_DIR}/mtds_lib/mutex_queue.hpp mtds_lib/mpsc_queue.hpp
		mtds_lib/details/tagged_ptr.hpp mtds_lib/mpsc_stack.hpp mtds_lib/details/free_list.hpp mtds_lib/mpmc_queue.hpp mtds_lib/mpmc_stack.hpp)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/mtds_lib)

##########################################################

target_link_libraries(${PROJECT_NAME} ${LIB_NAME})

##########################################################

#! Specify output directories
#! For different tests etc it should be `bin`. For shared library it should be `lib`
set_target_properties(${LIB_NAME} ${PROJECT_NAME}
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(${LIB_NAME}
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
		LINKER_LANGUAGE CXX)

#! Add external packages
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

##########################################################
# Fixed CMakeLists.txt part 
##########################################################

INSTALL(PROGRAMS
		$<TARGET_FILE:${PROJECT_NAME}>	# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
		DESTINATION bin)

# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${PROJECT_NAME} mutex_queue_test mpsc_queue_test mpmc_queue_test mpsc_stack_test
		mpmc_stack_test free_list_test)

# Include CMake setup
include(cmake/main-config.cmake)

##########################################################
# GoogleTest
##########################################################

include(FetchContent)
FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
		mutex_queue_test
		test/mutex_queue_test.cpp
		mtds_lib/mutex_queue.hpp test/container_tests.hpp)

target_include_directories(mutex_queue_test PRIVATE mtds_lib test)

target_link_libraries(
		mutex_queue_test
		gtest_main
		Threads::Threads
)

add_executable(
		mpsc_queue_test
		test/mpsc_queue_test.cpp
		mtds_lib/mpsc_queue.hpp test/container_tests.hpp)

target_include_directories(mpsc_queue_test PRIVATE mtds_lib test)

target_link_libraries(
		mpsc_queue_test
		gtest_main
		Threads::Threads
)

add_executable(
		mpmc_queue_test
		test/mpmc_queue_test.cpp
		mtds_lib/mpmc_queue.hpp test/container_tests.hpp)

target_include_directories(mpmc_queue_test PRIVATE mtds_lib test)

target_link_libraries(
		mpmc_queue_test
		gtest_main
		Threads::Threads
)

add_executable(
		mpsc_stack_test
		test/mpsc_stack_test.cpp
		mtds_lib/mpsc_stack.hpp test/container_tests.hpp)

target_include_directories(mpsc_stack_test PRIVATE mtds_lib test)

target_link_libraries(
		mpsc_stack_test
		gtest_main
		Threads::Threads
)

add_executable(
		mpmc_stack_test
		test/mpmc_stack_test.cpp
		mtds_lib/mpmc_stack.hpp test/container_tests.hpp)

target_include_directories(mpmc_stack_test PRIVATE mtds_lib test)

target_link_libraries(
		mpmc_stack_test
		gtest_main
		Threads::Threads
)

add_executable(
		free_list_test
		test/free_list_test.cpp
		mtds_lib/details/free_list.hpp test/container_tests.hpp)

target_include_directories(free_list_test PRIVATE mtds_lib test)

target_link_libraries(
		free_list_test
		gtest_main
		Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
