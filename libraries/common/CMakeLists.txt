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
    src/config/config.cpp
    src/logging/logger.cpp
    src/logging/logging_impl.cpp
    src/logging/sink_stdout.cpp
    src/logging/sink_string.cpp
    src/logging/sink_fs.cpp
    src/format/format.cpp
    src/utils/errors.cpp
)

# headers
set(HEADERS
    ${BOOST_DIR}
    ${LIBS_DIR}
    ${THIRDPARTY_LIBS}
    ./include
    ./src
)

# compile options
set(COMPILE_OPTIONS 
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
    -pthread
)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

# test sources
set(TEST_SOURCES
    tests/config.cpp
    tests/format.cpp
    tests/log.cpp
    tests/main.cpp
    ${SOURCES}
)

set(TESTS_NAME ${PROJECT_NAME}_tests)
add_executable(${TESTS_NAME} ${TEST_SOURCES})
target_include_directories(${TESTS_NAME} PRIVATE ${HEADERS})
target_compile_options(${TESTS_NAME} PRIVATE ${COMPILE_OPTIONS})
