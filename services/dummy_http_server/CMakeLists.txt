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
set(SOURCES main.cpp)

# build executable
add_executable(${PROJECT_NAME} ${SOURCES})

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)
target_link_libraries(${PROJECT_NAME} lib_common)
