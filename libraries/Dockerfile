# Build and test
FROM speeedwagon/boost-base:latest AS build

WORKDIR /usr

COPY ./libraries/ ./libraries

RUN cd libraries && \
    cmake . && \
    cmake --build .

RUN ./libraries/common/lib_common_tests
RUN ./libraries/components/lib_components_tests
RUN ./libraries/http/lib_http_tests
