#! /bin/bash
docker build -t boost-base:latest .
docker tag boost-base:latest speeedwagon/boost-base:latest
docker login -u speeedwagon --password-stdin
docker push speeedwagon/boost-base:latest