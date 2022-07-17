# zoo
[![Generic badge](https://img.shields.io/badge/purpose-education-orange.svg)](https://shields.io/)
[![Generic badge](https://img.shields.io/badge/lang-cpp-blue.svg)](https://shields.io/)
[![Linux](https://svgshare.com/i/Zhy.svg)](https://svgshare.com/i/Zhy.svg)
[![build and test](https://github.com/lilSpeedwagon/zoo/actions/workflows/build-cpp.yml/badge.svg)](https://github.com/lilSpeedwagon/zoo/actions/workflows/build-cpp.yml)

Modern C++ projects and web services are based on the powerful frameworks and libraries. 
Most of them are efficient and easy-to-use, and there is no need in an another "cool C++ framework". 
But I would like to implement some of these basic tools by myself for better understanding of "guts" of the modern web frameworks.
So it is a C++ pet project intended for the implementation of the basic web service, tools and environment from scratch (or almost from scratch).

Target web service is a simple CRUD API generator. As a service user I can define API schema and data model. Described CRUD API with the simple database will be automatically deployed.

## Framework core roadmap
- async HTTP server and client :white_check_mark:
- logger :white_check_mark:
- static configs :white_check_mark:
- global components system :white_check_mark:
- test system :white_check_mark:
- key-value database
- deploy system
- caches
- log storage service
- metrics storage service
- document database
- dynamic configs service
- auth service
- API gateway
- TBD

## CRUD API service roadmap
- API configs service
- CRUD API provider
- statistics service
- auth service
- API gateway

## Stack
- C++17
- [Boost 1.76.0](https://www.boost.org/)
- [CMake](https://cmake.org/)
- [Catch2](https://github.com/catchorg/Catch2)
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [pytest](https://docs.pytest.org/)

## Development env requirements
1. Ubuntu 18 and later. You can try Windows, but there may be some problems with libraries detection.
2. GCC with C++17 support, CMake (3.0+), Boost (1.76+)
`sudo apt install build-essential libssl-dev libboost-all-dev`
3. Python 3.10 and later.
4. VS code extensions:
    * C/C++
    * CMake
    * CMake tools
    * Python
    * Catch2 and Google Test Explorer

## Build and test
The project is divided into microservices and libs. You can find them in corresponding directories. To build, debug and run unit-tests open CMake project in root directory. To build docker container with service use `./build_docker <service_name>`. Acceptance tests (based on pytest) may be runned via `./run_tests <service_name>` (corresponding docker image must be built before).
