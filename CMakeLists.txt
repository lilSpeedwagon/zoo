cmake_minimum_required (VERSION 3.0)
 
project (self_made_zoo)
 
add_subdirectory (./services/dummy_http_server)
add_subdirectory (./services/api_config)
add_subdirectory (./libraries/http)
add_subdirectory (./libraries/common)
add_subdirectory (./libraries/components)