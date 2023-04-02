cmake_minimum_required(VERSION 3.0)

project(document_db_service)
set (CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS thread REQUIRED)

# setup libraries headers
set(LIBS_DIR ../../libraries)

# headers
set(HEADERS_DIR ./src)

# sources
set(SOURCES
    src/components/storage.cpp
    src/fs_sink/fs_sink.cpp
    src/handlers/create.cpp
    src/handlers/update.cpp
    src/handlers/delete.cpp
    src/handlers/get.cpp
    src/handlers/list.cpp
    src/models/document.cpp
    src/utils/response.cpp
    src/utils/request.cpp
    main.cpp
)

# build executable
add_executable(${PROJECT_NAME} ${SOURCES})

# setup libraries headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BOOST_INCLUDE_DIRS}
    ${LIBS_DIR}
    ${HEADERS_DIR}
)

# link libs
target_link_libraries(${PROJECT_NAME} lib_http)
target_link_libraries(${PROJECT_NAME} lib_common)
target_link_libraries(${PROJECT_NAME} lib_components)
target_link_libraries(${PROJECT_NAME} Boost::thread)

target_compile_options(${PROJECT_NAME} PRIVATE
    -pthread
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_CHRONO_HEADER_ONLY
)
