#! /bin/bash
container_id=$(docker run --rm -d --network=host api_config_service)
python3 -m pytest -vv
test_result=$?
if (test $test_result != 0)
then
    docker logs ${container_id}
fi
docker stop ${container_id} &
