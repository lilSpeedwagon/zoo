#! /bin/bash

docker build -f ./libraries/Dockerfile -t zoo_libs .
exit $?
