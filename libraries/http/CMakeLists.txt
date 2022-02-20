cmake_minimum_required(VERSION 3.0)

project(lib_http)
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
set(SOURCES ./src/http_server/http_server.cpp ./src/tcp_session/tcp_session.cpp)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# link libs
target_link_libraries(${PROJECT_NAME} lib_common)
