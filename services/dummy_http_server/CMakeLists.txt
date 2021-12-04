cmake_minimum_required(VERSION 3.0)

project(dummy_service)
set (CMAKE_CXX_STANDARD 17)
add_compile_options(-D BOOST_DATE_TIME_NO_LIB)

# setup libraries headers
set(LIBS_DIR ../../libraries)
set(BOOST_DIR ../../libraries/boost_1_76_0)

include_directories(${BOOST_DIR})
include_directories(${LIBS_DIR})

# headers

# sources
set(SOURCE_EXE main.cpp)

# build executable
add_executable(main ${SOURCE_EXE})
