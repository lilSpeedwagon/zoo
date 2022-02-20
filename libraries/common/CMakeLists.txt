cmake_minimum_required(VERSION 3.0)

project(lib_common)
set (CMAKE_CXX_STANDARD 17)
add_compile_options(-DBOOST_DATE_TIME_NO_LIB -DBOOST_CHRONO_HEADER_ONLY)

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
set(SOURCES
        src/logging/logger.cpp
        src/logging/logging_impl.cpp
        src/logging/sink_stdout.cpp
)

SET(Boost_INCLUDE_DIR ${BOOST_DIR})
SET(Boost_LIB_DIR ${BOOST_DIR})
SET(Boost_USE_STATIC_LIBS ON)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})
