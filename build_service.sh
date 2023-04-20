#! /bin/bash
service=$1
if [ -z ${service}]
then
    echo "missing required positional argument <service_name>"
    exit 1
fi

docker build -f ./services/${service}/Dockerfile -t ${service}_service .
exit $?
