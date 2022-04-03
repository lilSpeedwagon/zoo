cmake_minimum_required(VERSION 3.0)

project(lib_common)
set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED ENV{BOOST_ROOT})
    message(WARNING "Env var BOOST_ROOT is not declared. Boost lib may be not found.")
endif()

# libraries headers
set(LIBS_DIR ../../libraries)
set(THIRDPARTY_LIBS ${LIBS_DIR}/thirdparty)
set(BOOST_DIR $ENV{BOOST_ROOT})

# sources
set(SOURCES
    src/logging/logger.cpp
    src/logging/logging_impl.cpp
    src/logging/sink_stdout.cpp
    src/logging/sink_string.cpp
    src/logging/sink_fs.cpp
    src/format/format.cpp
    src/utils/errors.cpp
)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# setup include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BOOST_DIR}
    ${LIBS_DIR}
    ${THIRDPARTY_LIBS}
    ./include
    ./src
)

# tests
set(TEST_SOURCES
    tests/main.cpp
    tests/format.cpp
    tests/log.cpp
    ${SOURCES}
)
add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
    -pthread
)
