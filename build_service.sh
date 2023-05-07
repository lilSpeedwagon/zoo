#! /bin/bash
service=$1
if [ -z ${service}]
then
    echo "missing required positional argument <service_name>"
    exit 1
fi

./build_libraries.sh
libs_build_result=$?
if (test ${libs_build_result} != 0)
then
    echo 'Build libs failed'
    exit ${libs_build_result}
fi

docker build -f ./services/${service}/Dockerfile -t ${service}_service .
exit $?
