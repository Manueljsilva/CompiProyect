cmake_minimum_required(VERSION 3.29)
project(ProyectoCompi)

set(CMAKE_CXX_STANDARD 20)
#add_executable(Proyecoto svm.cpp svm_parser.cpp svm_run.cpp)
add_executable(ProyectoCompi exp.cpp
        parser.cpp environment.hh imp_codegen.cpp  imp_type_checker.cpp
        scanner.cpp token.cpp  visitor.cpp main.cpp imp_type.cpp imp_interpreter.cpp
        imp_value.cpp imp_value_visitor.hh)
