#! /bin/bash

set -e # exit on any error

echo "Building and test libraries..."
./build_libraries.sh ${lib}
echo "---------------------------------------------"
echo ""

declare -a services=("dummy" "document_db" "api_config")
for service in "${services[@]}"
do
    echo "Building ${service}_service..."
    ./build_service.sh ${service}
    echo "Running ${service}_service tests..."
    ./run_tests.sh ${service}
    echo "---------------------------------------------"
    echo ""
    sleep 5
done

echo "Done."
