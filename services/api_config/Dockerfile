# Build
FROM ubuntu:latest AS build

RUN apt update && \
    apt install build-essential libboost-all-dev cmake -y && \
    mkdir /usr/api_config_service
WORKDIR /usr/api_config_service

COPY ./services ./services
COPY ./libraries/ ./libraries
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cmake . && \
    cmake --build . --target api_config_service

# Run
FROM ubuntu:latest

RUN mkdir /api_config
WORKDIR /api_config
COPY --from=build \
    /usr/api_config_service/services/api_config/api_config_service \
    ./api_config_service
EXPOSE 80
CMD ./api_config_service