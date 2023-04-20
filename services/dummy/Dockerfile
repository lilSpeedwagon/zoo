# Build
FROM zoo_libs:latest AS build

WORKDIR /usr

COPY ./services ./services
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cmake . && \
    cmake --build . --target dummy_service

# Run
FROM ubuntu:latest

RUN mkdir /dummy
WORKDIR /dummy
COPY --from=build \
    /usr/services/dummy/dummy_service \
    ./dummy_service
EXPOSE 1111
CMD ./dummy_service
