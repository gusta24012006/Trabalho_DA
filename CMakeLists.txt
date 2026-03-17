cmake_minimum_required(VERSION 3.26)
project(untitled49)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled49 main.cpp
        Submission.h
        Reviewer.h
        Parameters.h
        Control.h
        Conference.h
        CsvReader.cpp
        CsvReader.h
        graph.cpp
        graph.h
        Assigner.cpp
        Assigner.h)
