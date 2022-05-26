#! /bin/bash
container_id=$(docker run --rm -d --network=host api_config_service)
python3 -m pytest
docker stop ${container_id}
