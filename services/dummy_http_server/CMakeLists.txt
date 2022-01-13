cmake_minimum_required(VERSION 3.0)

project(dummy_service)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_compile_options(-D BOOST_DATE_TIME_NO_LIB)

# build executable
add_executable(${PROJECT_NAME} main.cpp)

# setup boost
#find_package(Boost 1.65.0 REQUIRED)

# setup libraries headers
target_include_directories(${PROJECT_NAME} PRIVATE
     ../../libraries
     ../../libraries/boost_1_76_0
     #${Boost_INCLUDE_DIRS}
)

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -pedantic -Werror -pthread
)