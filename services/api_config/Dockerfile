# Build
FROM speeedwagon/boost-base:latest AS build

RUN mkdir /usr/api_config_service
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
COPY ./services/api_config/configs/log_config_default.json ./log_config.json
EXPOSE 80
CMD ./api_config_service