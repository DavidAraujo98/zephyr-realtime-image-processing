# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cabimageprocessing)

target_sources(app PRIVATE 
    src/imgProc/imgProcAlg.c
    src/cab/cab.c
    src/main.c
)