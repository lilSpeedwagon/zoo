cmake_minimum_required (VERSION 3.0)
 
project (self_made_zoo)

add_subdirectory (./libraries)

add_subdirectory (./services/api_config)
add_subdirectory (./services/document_db)
add_subdirectory (./services/dummy)
