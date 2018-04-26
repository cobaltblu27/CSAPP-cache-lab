cmake_minimum_required(VERSION 3.9)
project(cachelab_handout)

set(CMAKE_CXX_STANDARD 11)

add_executable(cachelab_handout
        cachelab.c
        cachelab.h
        csim.c
        test-trans.c
        tracegen.c
        trans.c)
