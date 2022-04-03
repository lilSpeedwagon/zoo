cmake_minimum_required(VERSION 3.0)

project(dummy_service)
set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED ENV{BOOST_ROOT})
    message(WARNING "Env var BOOST_ROOT is not declared. Boost lib may be not found.")
endif()

# setup libraries headers
set(LIBS_DIR ../../libraries)
set(BOOST_DIR $ENV{BOOST_ROOT})

# headers

# sources
set(SOURCES main.cpp)

# build executable
add_executable(${PROJECT_NAME} ${SOURCES})

# setup libraries headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BOOST_DIR}
    ${LIBS_DIR}
)

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)
target_link_libraries(${PROJECT_NAME} lib_common)

target_compile_options(${PROJECT_NAME} PRIVATE
    -pthread
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
)
