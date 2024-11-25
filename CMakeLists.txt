cmake_minimum_required(VERSION 3.29)
project(ProyectoCompi)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProyectoCompi main.cpp
        scanner.cpp
        token.cpp
        parser.cpp
        exp.cpp
        environment.hh
        imp_interpreter.cpp
        visitor.cpp
        imp_type.cpp
        imp_value.cpp
)
