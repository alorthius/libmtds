cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
#  It is used as your project's main executable name. 
set(LIB_NAME mtds)
project(${LIB_NAME} C CXX)

set(CMAKE_CXX_STANDARD 17)

##########################################################
# User configurable options of the template
##########################################################
# Note: symbols like WARNINGS_AS_ERRORS in configuration are intentionally variables
# and not CMake options --using options creates too much problem for students.

#! It is a good practice to set "WARNINGS_AS_ERRORS" ON,
#  but sometimes it creates too much trouble, so default is OFF.
set(WARNINGS_AS_ERRORS OFF)

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

set(detail_header_files
		${CMAKE_CURRENT_SOURCE_DIR}/include/mtds/details/tagged_ptr.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/mtds/details/backoff.hpp)

set(header_files
		${CMAKE_CURRENT_SOURCE_DIR}/include/mtds/mpmc_queue.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/mtds/mpmc_stack.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/mtds/mutex_queue.hpp)

add_library(${LIB_NAME} INTERFACE)
target_sources(${LIB_NAME} INTERFACE "$<BUILD_INTERFACE:${detail_header_files};${header_files}>")
target_include_directories(${LIB_NAME} INTERFACE include/)

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

set(test_targets
		mutex_queue_test
		mpmc_queue_test
		mpmc_stack_test
		tagged_ptr_test)

foreach(target ${test_targets})
	add_executable(${target} test/${target}.cpp)
	target_include_directories(${target} PRIVATE include/ test)
	target_link_libraries(${target} gtest_main Threads::Threads)
endforeach(target)

include(GoogleTest)
foreach(target ${test_targets})
	gtest_discover_tests(${target})
endforeach(target)

##########################################################
# Benchmark
##########################################################
find_package(benchmark REQUIRED)

add_executable(
		mtds_benchmark
		benchmark/mtds_benchmark.cpp)

target_link_libraries(mtds_benchmark benchmark::benchmark)

target_include_directories(mtds_benchmark PRIVATE benchmark include/)

##########################################################
# Fixed CMakeLists.txt part
##########################################################

install(PROGRAMS
		$<TARGET_FILE:mtds_benchmark>
		DESTINATION bin)

foreach(target ${test_targets})
	install(PROGRAMS
			$<TARGET_FILE:${target}>
			DESTINATION bin)
endforeach(target)

# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${test_targets})

# Include CMake setup
include(cmake/main-config.cmake)
