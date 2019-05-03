cmake_minimum_required(VERSION 3.13)
project(map)


set(CMAKE_CXX_STANDARD 14)



include_directories(.)
include_directories(data)
add_executable(map
        data/four/code.cpp
        #data/one.memcheck/code.cpp
        #data/three/code.cpp
        #data/three.memcheck/code.cpp
        #data/two/code.cpp
        #data/two.memcheck/code.cpp
        #data/class-bint.hpp
        #data/class-integer.hpp
        #data/class-matrix.hpp
        #exceptions.hpp
        utility.hpp
        map.hpp
        exceptions.hpp
        )
