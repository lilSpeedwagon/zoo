cmake_minimum_required (VERSION 3.0)
 
project (self_made_zoo)

# libraries
add_subdirectory (./libraries/common)
add_subdirectory (./libraries/components)
add_subdirectory (./libraries/http)

# services
add_subdirectory (./services/api_config)
add_subdirectory (./services/document_db)
add_subdirectory (./services/dummy)
