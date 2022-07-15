# Build
FROM speeedwagon/boost-base:latest AS build

RUN mkdir /usr/dummy_service
WORKDIR /usr/dummy_service

COPY ./services ./services
COPY ./libraries/ ./libraries
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cmake . && \
    cmake --build . --target dummy_service

# Run
FROM ubuntu:latest

RUN mkdir /dummy
WORKDIR /dummy
COPY --from=build \
    /usr/dummy_service/services/dummy/dummy_service \
    ./dummy_service
EXPOSE 1111
CMD ./dummy_service