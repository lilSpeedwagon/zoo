cmake_minimum_required(VERSION 3.0)

project(lib_http)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_compile_options(-D BOOST_DATE_TIME_NO_LIB)

# build static library
add_library(${PROJECT_NAME} STATIC
    ./src/http_server/http_server.cpp
    ./src/tcp_session/tcp_session.cpp
)

# setup boost
#find_package(Boost 1.65.0 REQUIRED)

# setup include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ../../libraries/boost_1_76_0
    ../../libraries
    ./include
    ./src
    #${Boost_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -pedantic -Werror -pthread
)
