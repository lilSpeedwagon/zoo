cmake_minimum_required(VERSION 3.0)

project(dummy_service)
set (CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

if(NOT DEFINED ENV{BOOST_INCLUDE_DIRS})
    message(WARNING "Env var BOOST_INCLUDE_DIRS is not declared. Boost lib may be not found.")
endif()

find_package(Threads REQUIRED)

# setup libraries headers
set(LIBS_DIR ../../libraries)

# headers

# sources
set(SOURCES main.cpp)

# build executable
add_executable(${PROJECT_NAME} ${SOURCES})

# setup libraries headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${LIBS_DIR}
)

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)
target_link_libraries(${PROJECT_NAME} lib_common)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

target_compile_options(${PROJECT_NAME} PRIVATE
    -lpthread
    -pthread
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
)
