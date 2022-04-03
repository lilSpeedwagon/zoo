cmake_minimum_required(VERSION 3.0)

project(lib_http)
set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED ENV{BOOST_ROOT})
    message(WARNING "Env var BOOST_ROOT is not declared. Boost lib may be not found.")
endif()

# libraries headers
set(LIBS_DIR ../../libraries)
set(THIRDPARTY_LIBS ${LIBS_DIR}/thirdparty)
set(BOOST_DIR $ENV{BOOST_ROOT})

# sources
set(SOURCES 
    ./src/http_server/http_server.cpp
    ./src/tcp_session/tcp_session.cpp
)

# build static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# link libs
target_link_libraries(${PROJECT_NAME} lib_common)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BOOST_DIR}
    ${LIBS_DIR}
    ${THIRDPARTY_LIBS}
    ./include
    ./src
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -pthread
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
)
