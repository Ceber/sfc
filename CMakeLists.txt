cmake_minimum_required(VERSION 3.13)

# Version
set(sfc_VERSION_MAJOR 0)
set(sfc_VERSION_MINOR 0)
set(sfc_VERSION_PATCH 2)
set(sfc_VERSION ${sfc_VERSION_MAJOR}.${sfc_VERSION_MINOR}.${sfc_VERSION_PATCH})
message(STATUS "####### sfc_VERSION:                    "   ${sfc_VERSION})

# set the project name and version
project(sfc VERSION ${sfc_VERSION} DESCRIPTION "C++ 'Sequential function chart' 2nd party lib.")
enable_language(CXX)

# Module and Packages
include(CMakePackageConfigHelpers)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Config)


############################################################################
# Configure and Find Packages
############################################################################
### Required Packages ###
# Threading
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Source files
file(GLOB_RECURSE ${PROJECT_NAME}_SRC
     "src/*.cpp"
)
# Source files
file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS
     "include/*.h*"
)

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SRC}
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_include_directories(${PROJECT_NAME} 
    PRIVATE      
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

if(BUILD_TESTS)
     enable_testing()
     # GTest
     find_package(GTest REQUIRED)
     # ... CMake code to create tests ...
     # Source files
     set(TESTS_EXE ${PROJECT_NAME}_unit-tests)
     file(GLOB_RECURSE ${PROJECT_NAME}_TESTS
          "unit-tests/**.h*"
          "unit-tests/**.cpp"
     )

     add_executable(${TESTS_EXE} ${${PROJECT_NAME}_TESTS})
     target_link_libraries(${TESTS_EXE} ${PROJECT_NAME} GTest::gtest)
     target_include_directories(${TESTS_EXE} 
          PRIVATE
               ${Python3_INCLUDE_DIRS}
     )
     set_target_properties (${TESTS_EXE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_OUTPUT_DIR})
     add_test(NAME ${TESTS_EXE} COMMAND $<TARGET_FILE:${TESTS_EXE}>)
endif(BUILD_TESTS)



############################################################################
# Settings + Directories + Export Install + CPack
############################################################################
# Install public include for developpers
install(DIRECTORY "include/" # source directory
        DESTINATION "${INSTALL_INCLUDE_DIR}" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h*" # select header files
)

include(Lib)


############################################################################
# Options Summary
############################################################################
message(STATUS "############## SFC OPTIONS SUMMARY ##############")
message(STATUS "####### BUILD_TESTS:                        " 	${BUILD_TESTS})
message(STATUS "####### BUILD_DEMOS:                        " 	${BUILD_DEMOS})
message(STATUS "####### BUILD_DOC:                          "    ${BUILD_DOC})
message(STATUS "####### CODE_COVERAGE:                      " 	${CODE_COVERAGE})
message(STATUS "####### CONAN_BUILD:                        " 	${CONAN_BUILD})
message(STATUS "####### ASAN:                               " 	${ASAN})
message(STATUS "####### IWYU:                               " 	${IWYU})

