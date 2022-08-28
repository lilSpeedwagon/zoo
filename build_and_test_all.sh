#! /bin/bash
set -e # exit on any error
declare -a services=("dummy" "document_db" "api_config")
for service in "${services[@]}"
do
    echo "Building ${service}_service..."
    ./build_docker.sh ${service}
    echo "Running ${service}_service tests..."
    ./run_tests.sh ${service}
    echo "---------------------------------------------"
    echo ""
    sleep 1
done
echo "Done."
