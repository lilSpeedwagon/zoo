cmake_minimum_required(VERSION 3.0)

project(lib_components)
set (CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

# libraries headers
set(LIBS_DIR ../../libraries)
set(THIRDPARTY_LIBS ${LIBS_DIR}/thirdparty)

# sources
set(SOURCES
    src/components_base.cpp
    src/components_controller.cpp
    src/components_engine.cpp
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

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)
target_link_libraries(${PROJECT_NAME} lib_common)

# test sources
set(TEST_SOURCES
    tests/components.cpp
    tests/main.cpp
    ${SOURCES}
)

# build tests
set(TESTS_NAME ${PROJECT_NAME}_tests)
add_executable(${TESTS_NAME} ${TEST_SOURCES})
target_include_directories(${TESTS_NAME} PRIVATE ${HEADERS})
target_compile_options(${TESTS_NAME} PRIVATE ${COMPILE_OPTIONS})

# link tests libs
target_link_libraries(${TESTS_NAME} lib_http)
target_link_libraries(${TESTS_NAME} lib_common)

