cmake_minimum_required(VERSION 3.0)

project(lib_http)
set (CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

if(NOT DEFINED ENV{BOOST_INCLUDE_DIRS})
    message(WARNING "Env var BOOST_INCLUDE_DIRS is not declared. Boost lib may be not found.")
endif()

# libraries headers
set(LIBS_DIR ../../libraries)
set(THIRDPARTY_LIBS ${LIBS_DIR}/thirdparty)

# sources
set(SOURCES 
    ./src/http_server/http_server.cpp
    ./src/http_client/http_client.cpp
    ./src/tcp_session/tcp_session.cpp
    ./src/utils/utils.cpp
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
    -pthread
)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# link libs
target_link_libraries(${PROJECT_NAME} lib_common)
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})

# test sources
set(TEST_SOURCES
    tests/main.cpp
    tests/utils.cpp
    ${SOURCES}
)

set(TESTS_NAME ${PROJECT_NAME}_tests)
add_executable(${TESTS_NAME} ${TEST_SOURCES})
target_link_libraries(${TESTS_NAME} lib_common)
target_include_directories(${TESTS_NAME} PRIVATE ${HEADERS})
target_compile_options(${TESTS_NAME} PRIVATE ${COMPILE_OPTIONS})
