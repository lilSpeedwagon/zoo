cmake_minimum_required(VERSION 3.0)

project(lib_common)
set (CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS regex REQUIRED)

# libraries headers
set(LIBS_DIR ../../libraries)
set(THIRDPARTY_LIBS ${LIBS_DIR}/thirdparty)

# sources
set(SOURCES
    src/binary/binary.cpp
    src/config/config.cpp
    src/config/logging_config.cpp
    src/logging/logger.cpp
    src/logging/logging_impl.cpp
    src/logging/sink_stdout.cpp
    src/logging/sink_string.cpp
    src/logging/sink_fs.cpp
    src/format/format.cpp
    src/threading/thread_pool.cpp
    src/utils/errors.cpp
)

# headers
set(HEADERS
    ${BOOST_INCLUDE_DIRS}
    ${LIBS_DIR}
    ${THIRDPARTY_LIBS}
    ./include
    ./src
)

# compile options
set(COMPILE_OPTIONS 
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
    -lboost_regex
    -pthread
)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# test sources
set(TEST_SOURCES
    tests/binary.cpp
    tests/config.cpp
    tests/format.cpp
    tests/log.cpp
    tests/main.cpp
    tests/strong_typedef.cpp
    tests/utils.cpp
    ${SOURCES}
)

set(TESTS_NAME ${PROJECT_NAME}_tests)
add_executable(${TESTS_NAME} ${TEST_SOURCES})
target_include_directories(${TESTS_NAME} PRIVATE ${HEADERS})
target_compile_options(${TESTS_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(${TESTS_NAME} Boost::regex Threads::Threads)
