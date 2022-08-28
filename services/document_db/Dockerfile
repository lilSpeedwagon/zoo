# Build
FROM speeedwagon/boost-base:latest AS build

RUN mkdir /usr/document_db_service
WORKDIR /usr/document_db_service

COPY ./services ./services
COPY ./libraries/ ./libraries
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cmake . && \
    cmake --build . --target document_db_service

# Run
FROM speeedwagon/boost-base:latest

RUN mkdir /document_db_service
WORKDIR /document_db_service
COPY --from=build \
    /usr/document_db_service/services/document_db/document_db_service \
    ./document_db_service
COPY ./services/document_db/configs/log_config_default.json ./log_config.json
EXPOSE 80
CMD ./document_db_service
