#! /bin/bash
service=$1
if [ -z ${service}]
then
    echo "missing required positional argument <service_name>"
    exit 1
fi

container_id=$(docker run --rm -d --network=host ${service}_service)
cd services/${service}
python3 -m pytest -vv
test_result=$?
if (test $test_result != 0)
then
    docker logs ${container_id}
fi
docker stop ${container_id} &
