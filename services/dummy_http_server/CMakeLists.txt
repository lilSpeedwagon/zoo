cmake_minimum_required(VERSION 3.0)

project(dummy_service)
set (CMAKE_CXX_STANDARD 17)
add_compile_options(-DBOOST_DATE_TIME_NO_LIB -DBOOST_CHRONO_HEADER_ONLY)

if(NOT DEFINED ENV{BOOST_ROOT})
        message(WARNING "Env var BOOST_ROOT is not declared. Boost lib may be not found.")
endif()

# setup libraries headers
set(LIBS_DIR ../../libraries)
set(BOOST_DIR $ENV{BOOST_ROOT})

include_directories(${BOOST_DIR})
include_directories(${LIBS_DIR})

# headers

# sources
set(SOURCES main.cpp)

# build executable
add_executable(${PROJECT_NAME} ${SOURCES})

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)
target_link_libraries(${PROJECT_NAME} lib_common)
