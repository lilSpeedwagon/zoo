cmake_minimum_required(VERSION 3.0)

project(lib_common)
set (CMAKE_CXX_STANDARD 17)
add_compile_options(-DBOOST_DATE_TIME_NO_LIB -DBOOST_CHRONO_HEADER_ONLY)

if(NOT DEFINED ENV{BOOST_ROOT})
        message(WARNING "Env var BOOST_ROOT is not declared. Boost lib may be not found.")
endif()

# libraries headers
set(LIBS_DIR ../../libraries)
set(THIRDPARTY_LIBS ${LIBS_DIR}/thirdparty)
set(BOOST_DIR $ENV{BOOST_ROOT})

# http lib headers
set(PUBLIC_HEADERS ./include)
set(PRIVATE_HEADERS ./src)

# setup include directories
set(HEADER_DIRS ${LIBS_DIR} ${BOOST_DIR} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${THIRDPARTY_LIBS})
include_directories(${HEADER_DIRS})

# sources
set(SOURCES
        src/logging/logger.cpp
        src/logging/logging_impl.cpp
        src/logging/sink_stdout.cpp
        src/logging/sink_string.cpp
        src/format/format.cpp
)

set(Boost_INCLUDE_DIR ${BOOST_DIR})
set(Boost_LIB_DIR ${BOOST_DIR})
set(Boost_USE_STATIC_LIBS ON)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# tests
set(TEST_SOURCES
        tests/main.cpp
        tests/format.cpp
        tests/log.cpp
        ${SOURCES}
)
add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

