cmake_minimum_required(VERSION 3.0)

project(lib_common)
set (CMAKE_CXX_STANDARD 17)
add_compile_options(-D BOOST_DATE_TIME_NO_LIB)

# libraries headers
set(LIBS_DIR ../../libraries)
set(BOOST_DIR ../../libraries/boost_1_76_0)

# http lib headers
set(PUBLIC_HEADERS ./include)
set(PRIVATE_HEADERS ./src)

# setup include directories
set(HEADER_DIRS ${LIBS_DIR} ${BOOST_DIR} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
include_directories(${HEADER_DIRS})

# sources
set(SOURCES src/logging/logging.cpp)

SET(Boost_INCLUDE_DIR ${BOOST_DIR})
SET(Boost_LIB_DIR ${BOOST_DIR})
SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost 1.76 COMPONENTS log REQUIRED)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})
