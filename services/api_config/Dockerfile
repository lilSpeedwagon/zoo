# Build
FROM zoo_libs:latest AS build

WORKDIR /usr

COPY ./services ./services
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cmake . && \
    cmake --build . --target api_config_service

# Run
FROM ubuntu:latest

RUN mkdir /api_config
WORKDIR /api_config
COPY --from=build \
    /usr/services/api_config/api_config_service \
    ./api_config_service
COPY ./services/api_config/configs/log_config_default.json ./log_config.json
EXPOSE 80
CMD ./api_config_service
